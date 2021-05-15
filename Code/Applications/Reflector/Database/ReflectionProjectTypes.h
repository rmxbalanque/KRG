#pragma once

#include "System/Core/Types/StringID.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Reflection
{
    typedef StringID HeaderID;
    typedef StringID ProjectID;

    //-------------------------------------------------------------------------

    struct HeaderInfo
    {
        inline static HeaderID GetHeaderID( FileSystem::Path const& headerFilePath )
        {
            String lowercasePath = headerFilePath;
            lowercasePath.make_lower();
            return StringID( lowercasePath.c_str() );
        }

    public:

        HeaderID                        m_ID;
        ProjectID                       m_projectID;
        String                          m_filePath;
        uint64                          m_timestamp = 0;
        uint64                          m_checksum = 0;
    };

    //-------------------------------------------------------------------------

    struct ProjectInfo
    {
        inline static ProjectID GetProjectID( FileSystem::Path const& projectFilePath )
        {
            String lowercasePath = projectFilePath;
            lowercasePath.make_lower();
            return StringID( lowercasePath.c_str() );
        }

        inline HeaderInfo const& GetModuleHeaderDesc() const
        {
            KRG_ASSERT( m_moduleHeaderID.IsValid() );
            for ( auto const& headerDesc : m_headerFiles )
            {
                if ( headerDesc.m_ID == m_moduleHeaderID )
                {
                    return headerDesc;
                }
            }

            KRG_UNREACHABLE_CODE();
            static HeaderInfo const temp;
            return temp;
        }

    public:

        ProjectID                       m_ID;
        String                          m_name;
        FileSystem::Path                m_path;
        String                          m_exportMacro;
        HeaderID                        m_moduleHeaderID;
        uint32                          m_dependencyCount = 0;
        TVector<HeaderInfo>             m_headerFiles;
        TVector<uint32>                 m_dirtyHeaders;
        TVector<ProjectID>              m_dependencies;
        mutable String                  m_moduleClassName;
        bool                            m_includeInAutogenerateModuleRegistrationList = true;
    };

    //-------------------------------------------------------------------------

    struct SolutionInfo
    {
        FileSystem::Path                m_path;
        TVector<ProjectInfo>            m_projects;
    };
}