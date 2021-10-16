#include "ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    CompileContext::CompileContext( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& compiledResourceDirectoryPath, ResourceID const& resourceToCompile )
        : m_typeRegistry( typeRegistry )
        , m_rawResourceDirectoryPath( rawResourceDirectoryPath )
        , m_compiledResourceDirectoryPath( compiledResourceDirectoryPath )
        , m_resourceID( resourceToCompile )
    {
        KRG_ASSERT( m_rawResourceDirectoryPath.IsDirectoryPath() && m_rawResourceDirectoryPath.Exists() && resourceToCompile.IsValid() );

        // Resolve paths
        ResourcePath const& resourceToCompilePath = resourceToCompile.GetPath();
        const_cast<FileSystem::Path&>( m_inputFilePath ) = ResourcePath::ToFileSystemPath( m_rawResourceDirectoryPath, resourceToCompilePath );
        const_cast<FileSystem::Path&>( m_outputFilePath ) = ResourcePath::ToFileSystemPath( m_compiledResourceDirectoryPath, resourceToCompilePath );
    }

    bool CompileContext::IsValid() const
    {
        if ( !m_inputFilePath.IsValid() || !m_outputFilePath.IsValid() || !m_resourceID.IsValid() )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    CompilationResult Compiler::Error( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Error, m_name.c_str(), __FILE__, __LINE__, pFormat, args );
        va_end( args );
        return CompilationResult::Failure;
    }

    CompilationResult Compiler::Warning( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Warning, m_name.c_str(), __FILE__, __LINE__, pFormat, args );
        va_end( args );
        return CompilationResult::SuccessWithWarnings;
    }

    CompilationResult Compiler::Message( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Message, m_name.c_str(), __FILE__, __LINE__, pFormat, args );
        va_end( args );
        return CompilationResult::Success;
    }

    CompilationResult Compiler::CompilationSucceeded( CompileContext const& ctx ) const
    {
        return Message( "Compiled '%s' to '%s' successfully", (char const*) ctx.m_inputFilePath, (char const*) ctx.m_outputFilePath );
    }

    CompilationResult Compiler::CompilationSucceededWithWarnings( CompileContext const& ctx ) const
    {
        Message( "Compiled '%s' to '%s' successfully", (char const*) ctx.m_inputFilePath, (char const*) ctx.m_outputFilePath );
        return CompilationResult::SuccessWithWarnings;
    }

    CompilationResult Compiler::CompilationFailed( CompileContext const& ctx ) const
    {
        return Error( "Failed to compile resource: '%s'", (char const*) ctx.m_outputFilePath );
    }
}