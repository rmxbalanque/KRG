#include "ClangParser.h"
#include "ClangVisitors_TranslationUnit.h"
#include "Applications/Reflector/ReflectorSettingsAndUtils.h"
#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/Platform/PlatformHelpers_Win32.h"
#include <fstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            ClangParser::ClangParser( SolutionInfo* pSolution, ReflectionDatabase* pDatabase, FileSystem::Path const& reflectionDataPath )
                : m_context( pSolution, pDatabase )
                , m_totalParsingTime( 0 )
                , m_totalVisitingTime( 0 )
                , m_reflectionDataPath( reflectionDataPath )
            {}

            bool ClangParser::Parse( TVector<HeaderInfo*> const& headers )
            {
                // Create single amalgamated header file for all headers to parse
                //-------------------------------------------------------------------------

                std::ofstream reflectorFileStream;
                FileSystem::Path const reflectorHeader = m_reflectionDataPath + "Reflector.h";
                FileSystem::EnsurePathExists( reflectorHeader );
                reflectorFileStream.open( reflectorHeader.c_str(), std::ios::out | std::ios::trunc );
                KRG_ASSERT( !reflectorFileStream.fail() );

                String includeStr;
                for ( HeaderInfo const* pHeader : headers )
                {
                    m_context.m_headersToVisit.push_back( pHeader->m_ID );
                    includeStr += "#include \"" + pHeader->m_filePath + "\"\n";
                }

                reflectorFileStream.write( includeStr.c_str(), includeStr.size() );
                reflectorFileStream.close();

                // Clang args
                TInlineVector<String, 10> fullIncludePaths;
                TInlineVector<char const*, 10> clangArgs;
                int32 const numIncludePaths = sizeof( Settings::g_includePaths ) / sizeof( Settings::g_includePaths[0] );
                for ( auto i = 0; i < numIncludePaths; i++ )
                {
                    String const fullPath = m_context.m_pSolution->m_path + Settings::g_includePaths[i];
                    String const shortPath = Platform::Win32::GetShortPath( fullPath );
                    fullIncludePaths.push_back( "-I" + shortPath );
                    clangArgs.push_back( fullIncludePaths.back().c_str() );
                }

                clangArgs.push_back( "-x" );
                clangArgs.push_back( "c++" );
                clangArgs.push_back( "-std=c++17" );
                clangArgs.push_back( "-O0" );
                clangArgs.push_back( "-D NDEBUG" );
                clangArgs.push_back( "-Werror" );
                clangArgs.push_back( "-Wno-unknown-warning-option" );
                clangArgs.push_back( "-Wno-return-type-c-linkage" );
                clangArgs.push_back( "-Wno-gnu-folding-constant" );

                // Set up clang
                auto idx = clang_createIndex( 0, 1 );
                uint32 const clangOptions = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies;

                // Parse Headers
                CXTranslationUnit tu;
                CXErrorCode result = CXError_Failure;
                {
                    ScopedSystemTimer timer( m_totalParsingTime );
                    result = clang_parseTranslationUnit2( idx, reflectorHeader.c_str(), clangArgs.data(), clangArgs.size(), 0, 0, clangOptions, &tu );
                }

                if ( result == CXError_Success )
                {
                    ScopedSystemTimer timer( m_totalVisitingTime );
                    m_context.Reset( &tu );
                    auto cursor = clang_getTranslationUnitCursor( tu );
                    clang_visitChildren( cursor, VisitTranslationUnit, &m_context );
                }
                clang_disposeIndex( idx );

                // Check for error
                if ( result != CXError_Success || m_context.ErrorOccured() )
                {
                    if ( m_context.ErrorOccured() )
                    {
                        m_context.LogError( "%s --> %s", reflectorHeader.c_str(), m_context.GetErrorMessage() );
                    }
                    return false;
                }

                return true;
            }
        }
    }
}