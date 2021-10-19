#pragma once
#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "System/Resource/ResourcePath.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class KRG_TOOLS_CORE_API ResourceDatabase : public FileSystem::IFileSystemChangeListener
    {
        struct ResourceRecord
        {
            ResourcePath                                            m_path;
            FileSystem::Path                                        m_filePath;
            ResourceTypeID                                          m_resourceTypeID;
        };

    private:

        struct Directory
        {
            void ChangePath( FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& newPath );
            void Clear();
            bool IsEmpty() const { return m_directories.empty() && m_files.empty(); }

        public:

            StringID                                                m_name;
            FileSystem::Path                                        m_filePath;
            TVector<Directory>                                      m_directories;
            TVector<ResourceRecord*>                                m_files;
        };

    public:

        ~ResourceDatabase();

        inline bool IsInitialized() const { return m_pTypeRegistry != nullptr; }
        void Initialize( TypeSystem::TypeRegistry const* pTypeRegistry, FileSystem::Path const& rawResourceDirPath );
        void Shutdown();
        void Update();

        // Query interface
        //-------------------------------------------------------------------------

        void GetAllResourcesOfType( ResourceTypeID typeID, TVector<ResourceRecord*>& outResources ) const;

    private:

        void RebuildDatabase();
        void ClearDatabase();

        // Directory operations
        Directory* FindDirectory( FileSystem::Path const& dirPath );
        Directory* FindOrCreateDirectory( FileSystem::Path const& dirPath );

        // Add/Remove records
        void AddFileRecord( FileSystem::Path const& path );
        void RemoveFileRecord( FileSystem::Path const& path );

        // File system listener
        virtual void OnFileCreated( FileSystem::Path const& path ) override final;
        virtual void OnFileDeleted( FileSystem::Path const& path ) override final;
        virtual void OnFileRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override final;
        virtual void OnDirectoryCreated( FileSystem::Path const& path ) override final;
        virtual void OnDirectoryDeleted( FileSystem::Path const& path ) override final;
        virtual void OnDirectoryRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override final;

    private:

        TypeSystem::TypeRegistry const*                             m_pTypeRegistry;
        FileSystem::Path                                            m_rawResourceDirPath;
        int32                                                       m_dataDirectoryPathDepth;
        FileSystem::FileSystemWatcher                               m_fileSystemWatcher;

        Directory                                                   m_rootDir;
        THashMap<ResourceTypeID, TVector<ResourceRecord*>>          m_resourcesPerType;
    };
}