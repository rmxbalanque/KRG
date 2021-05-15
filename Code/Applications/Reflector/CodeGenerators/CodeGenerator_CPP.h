#pragma once

#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include <sstream>

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG::CPP
{
    class Generator
    {
    public:

        Generator() : m_pDatabase( nullptr ) {}
        ~Generator() {}
        bool Generate( ReflectionDatabase const& database, SolutionInfo const& solution );
        char const* GetErrorMessage() const { return m_errorMessage.c_str(); }

    private:

        // File specific functions
        void GenerateHeaderFileHeader( HeaderInfo const& hdr, String const& headerID );
        void GenerateCodeFileHeader( String const& generatedHeaderFilePath );
        void GenerateModuleCodeFile( ProjectInfo const& prj, TVector<ReflectedType> const& typesInModule );

        // Utils
        static bool SaveStreamToFile( FileSystem::Path const& filePath, std::stringstream& stream );
        bool LogError( char const* pErrorFormat, ... ) const;

    private:

        ReflectionDatabase const*           m_pDatabase;
        std::stringstream                   m_headerFile;
        std::stringstream                   m_cppFile;
        std::stringstream                   m_moduleFile;
        std::stringstream                   m_engineTypeRegistrationFile;
        std::stringstream                   m_toolsTypeRegistrationFile;
        mutable String                      m_errorMessage;
    };
}