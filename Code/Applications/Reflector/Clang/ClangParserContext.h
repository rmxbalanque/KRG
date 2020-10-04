#pragma once

#include "ClangUtils.h"
#include "Applications/Reflector/ReflectorSettingsAndUtils.h"
#include "Applications/Reflector/Database/ReflectionProjectTypes.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            class ReflectionDatabase;

            //-------------------------------------------------------------------------

            struct TypeRegistrationMacro
            {

            public:

                TypeRegistrationMacro()
                    : m_type( ReflectionMacro::Unknown )
                    , m_position( 0xFFFFFFFF )
                {}

                TypeRegistrationMacro( ReflectionMacro type, CXCursor cursor, CXSourceRange sourceRange );

                inline bool IsValid() const { return m_type != ReflectionMacro::Unknown; }
                inline bool IsModuleMacro() const { return m_type == ReflectionMacro::RegisterModule; }
                inline bool IsEntityComponentMacro() const { return m_type == ReflectionMacro::RegisterEntityComponent; }
                inline bool IsEntitySystemMacro() const { return m_type == ReflectionMacro::RegisterEntitySystem; }
                inline bool IsTypeMacro() const { return m_type == ReflectionMacro::RegisterType; }
                inline bool IsResourceMacro() const { return m_type == ReflectionMacro::RegisterResource; }
                inline bool IsEnumMacro() const { return m_type == ReflectionMacro::RegisterEnum; }

            public:

                U32                 m_position;
                ReflectionMacro     m_type;
                String              m_macroContents;
            };

            //-------------------------------------------------------------------------

            struct ExposedPropertyMacro
            {
                ExposedPropertyMacro( HeaderID ID, U32 line )
                    : m_headerID( ID )
                    , m_lineNumber( line )
                {}

                bool operator==( ExposedPropertyMacro const& rhs )
                {
                    return m_headerID == rhs.m_headerID && m_lineNumber == rhs.m_lineNumber;
                }

                bool operator!=( ExposedPropertyMacro const& rhs )
                {
                    return m_headerID != rhs.m_headerID || m_lineNumber != rhs.m_lineNumber;
                }

            public:

                HeaderID        m_headerID;
                U32             m_lineNumber;
            };

            //-------------------------------------------------------------------------

            class ClangParserContext
            {

            public:

                ClangParserContext( SolutionDesc* pSolution, ReflectionDatabase* pDatabase )
                    : m_pTU( nullptr )
                    , m_pDatabase( pDatabase )
                    , m_pCurrentEntry( nullptr )
                    , m_inEngineNamespace( false )
                    , m_pSolution( pSolution )
                {
                    KRG_ASSERT( pSolution != nullptr && pDatabase != nullptr );
                }

                void LogError( char const* pFormat, ... ) const;
                char const* GetErrorMessage() const { return m_errorMessage.c_str(); }
                inline bool ErrorOccured() const { return !m_errorMessage.empty(); }

                bool ShouldVisitHeader( HeaderID headerID ) const;

                void Reset( CXTranslationUnit* pTU );
                void PushNamespace( String const& name );
                void PopNamespace();

                bool IsValidModuleName( String const& moduleClassName );
                bool SetModuleClassName( FileSystemPath const& headerFilePath, String const& moduleClassName );
                inline StringID GenerateTypeID( String const& fullyQualifiedTypeName ) const { return StringID( fullyQualifiedTypeName ); }

                String const& GetCurrentNamespace() const { return m_currentNamespace; }
                bool IsInEngineNamespace() const { return m_inEngineNamespace; }
                bool IsEngineNamespace( String const& namespaceString ) const;

                // Type Registration
                void AddFoundTypeRegistrationMacro( TypeRegistrationMacro const& foundMacro ) { m_typeRegistrationMacros.push_back( foundMacro ); }
                void AddFoundExposedPropertyMacro( ExposedPropertyMacro const& foundMacro ) { m_exposedPropertyMacros.push_back( foundMacro ); }
                bool ShouldRegisterType( CXCursor const& cr, TypeRegistrationMacro* pMacro = nullptr );

            public:

                CXTranslationUnit*                  m_pTU;

                // Per solution
                SolutionDesc*                       m_pSolution;
                ReflectionDatabase*                 m_pDatabase;
                TVector<HeaderID>                   m_headersToVisit;

                // Per Translation unit
                void*                               m_pCurrentEntry;
                TVector<ExposedPropertyMacro>       m_exposedPropertyMacros;
                TVector<TypeRegistrationMacro>      m_typeRegistrationMacros;

            private:

                mutable String                      m_errorMessage;
                TVector<String>                     m_namespaceStack;
                TVector<String>                     m_structureStack;
                String                              m_currentNamespace;
                bool                                m_inEngineNamespace;
            };
        }
    }
}