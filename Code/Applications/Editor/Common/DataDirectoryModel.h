#pragma once

#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------
// Data Directory Model
//-------------------------------------------------------------------------
// Provides a (filtered) view on the KRG data directory
// Provides filesystem access to be able to rename/delete/add files
// Provides UI state management (expansion, selection, etc...)

namespace KRG
{
    class UpdateContext;

    //-------------------------------------------------------------------------

    class DataDirectoryModel final : public FileSystem::IFileSystemChangeListener
    {
    public:

        struct File
        {
            File() = default;

            File( FileSystem::Path const& path )
                : m_name( path.GetFileName() )
                , m_path( path )
                , m_resourceTypeID( path.GetExtension() )
            {
                KRG_ASSERT( path.IsValid() );
            }

            inline bool operator==( FileSystem::Path const& path ) const { return m_path == path; }
            inline operator eastl_size_t() const { return m_path.GetHashCode(); }

            inline bool IsHidden() const { return m_isHidden; }

        public:

            String                                  m_name;
            FileSystem::Path                        m_path;
            ResourceTypeID                          m_resourceTypeID;

            mutable bool                            m_isHidden = false;
        };

        struct Directory
        {
            Directory() = default;

            Directory( char const* pName, FileSystem::Path const& path )
                : m_name( pName )
                , m_path( path )
            {
                KRG_ASSERT( !m_name.empty() );
                KRG_ASSERT( path.IsValid() && path.Exists() );
            }

            inline bool operator==( String const& name ) const { return m_name == name; }
            inline operator eastl_size_t() const { return eastl::hash<String>()(m_name); }

            inline bool IsHidden() const 
            {
                for ( auto const& dir : m_directories )
                {
                    if ( !dir.IsHidden() )
                    {
                        return false;
                    }
                }

                for ( auto const& file : m_files )
                {
                    if ( !file.IsHidden() )
                    {
                        return false;
                    }
                }
                
                return true;
            }

            inline void ForEachFile( TFunction<void( File const& file )> const& function ) const
            {
                for ( auto& dir : m_directories )
                {
                    dir.ForEachFile( function );
                }

                for ( auto& file : m_files )
                {
                    function( file );
                }
            }

            inline void ForEachDirectory( TFunction<void( Directory const& dir )> const& function ) const
            {
                for ( auto& dir : m_directories )
                {
                    function( dir );
                    const_cast<Directory&>( dir ).ForEachDirectory( function );
                }
            }

        public:

            String                                  m_name;
            FileSystem::Path                        m_path;
            THashSet<Directory>                     m_directories;
            THashSet<File>                          m_files;

            mutable bool                            m_isExpanded = false;
        };

    public:

        DataDirectoryModel();

        void Initialize( char const* const filter[] = {} );
        void Shutdown();
        void Update( UpdateContext const& context );

        inline TVector<FileSystem::Path> const& GetAllPaths() const { return m_foundPaths; }
        inline Directory const& GetRootDirectory() const { return m_rootDirectory; }

        inline bool IsSelected( FileSystem::Path const& item ) const { return m_selectedItem == item; }
        inline FileSystem::Path const& GetSelection() const { return m_selectedItem; }
        inline void SetSelection( FileSystem::Path const& selectedItem ) { m_selectedItem = selectedItem; }
        inline void ClearSelection() { m_selectedItem = FileSystem::Path(); }

        void ForEachFile( TFunction<void( File const& file )> const& function ) const { m_rootDirectory.ForEachFile( function ); }
        void ForEachDirectory( TFunction<void( Directory const& dir )> const& function ) const { m_rootDirectory.ForEachDirectory( function ); }

    private:

        // File system listener
        virtual void OnFileCreated( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnFileDeleted( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnFileRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override { Refresh(); }
        virtual void OnDirectoryCreated( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnDirectoryDeleted( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnDirectoryRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override { Refresh(); }

        void Refresh() {}

        Directory& FindOrCreateDirectoryForFile( FileSystem::Path const& path );

    private:

        FileSystem::Path                            m_dataDirectoryPath;
        int32                                       m_dataDirectoryPathDepth;
        FileSystem::FileSystemWatcher               m_fileSystemWatcher;

        TVector<FileSystem::Path>                   m_foundPaths;
        Directory                                   m_rootDirectory;

        FileSystem::Path                            m_selectedItem;
    };
}