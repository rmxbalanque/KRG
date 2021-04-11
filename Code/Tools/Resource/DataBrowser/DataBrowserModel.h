#pragma once

#include "Tools/Resource/_Module/API.h"
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

    class KRG_TOOLS_RESOURCE_API DataFileModel
    {
        friend class DataBrowserModel;

    public:

        DataFileModel() = default;

        DataFileModel( FileSystem::Path const& path )
            : m_name( path.GetFileName() )
            , m_path( path )
            , m_resourceTypeID( path.GetExtension() )
        {
            KRG_ASSERT( path.IsValid() );
        }

        inline String const& GetName() const { return m_name; }
        inline FileSystem::Path const& GetPath() const { return m_path; }
        inline ResourceTypeID const& GetResourceTypeID() const { return m_resourceTypeID; }

        inline void SetVisible( bool isVisible ) { m_isVisible = isVisible; }
        inline bool IsVisible() const { return m_isVisible; }

    private:

        String                                  m_name;
        FileSystem::Path                        m_path;
        ResourceTypeID                          m_resourceTypeID;
        bool                                    m_isVisible = true;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_RESOURCE_API DataDirectoryModel
    {
        friend class DataBrowserModel;

    public:

        DataDirectoryModel() = default;

        DataDirectoryModel( char const* pName, FileSystem::Path const& path )
            : m_name( pName )
            , m_path( path )
        {
            KRG_ASSERT( !m_name.empty() );
            KRG_ASSERT( path.IsValid() && path.Exists() );
        }

        inline String const& GetName() const { return m_name; }
        inline FileSystem::Path const& GetPath() const { return m_path; }

        inline TVector<DataDirectoryModel>& GetDirectories() { return m_directories; }
        inline TVector<DataDirectoryModel> const& GetDirectories() const { return m_directories; }

        inline TVector<DataFileModel>& GetFiles() { return  m_files; }
        inline TVector<DataFileModel> const& GetFiles() const { return  m_files; }

        inline void SetExpanded( bool isExpanded ) { m_isExpanded = isExpanded; }
        inline bool IsExpanded() const { return m_isExpanded; }

        inline bool IsVisible() const
        {
            for ( auto const& dir : m_directories )
            {
                if ( dir.IsVisible() )
                {
                    return true;
                }
            }

            for ( auto const& file : m_files )
            {
                if ( file.IsVisible() )
                {
                    return true;
                }
            }

            return false;
        }

        inline void ForEachFile( TFunction<void( DataFileModel const& file )> const& function ) const
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

        inline void ForEachDirectory( TFunction<void( DataDirectoryModel const& dir )> const& function ) const
        {
            for ( auto& dir : m_directories )
            {
                function( dir );
                dir.ForEachDirectory( function );
            }
        }

        inline void ForEachDirectory( TFunction<void( DataDirectoryModel& dir )> const& function )
        {
            for ( auto& dir : m_directories )
            {
                function( dir );
                dir.ForEachDirectory( function );
            }
        }

        inline void UpdateFileVisibility( TFunction<bool( DataFileModel const& file )> const& isVisibleFunction )
        {
            for ( auto& dir : m_directories )
            {
                dir.UpdateFileVisibility( isVisibleFunction );
            }

            for ( auto& file : m_files )
            {
                file.SetVisible( isVisibleFunction( file ) );
            }
        }

    private:

        String                                  m_name;
        FileSystem::Path                        m_path;
        TVector<DataDirectoryModel>             m_directories;
        TVector<DataFileModel>                  m_files;

        bool                                    m_isExpanded = false;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_RESOURCE_API DataBrowserModel final : public FileSystem::IFileSystemChangeListener
    {

    public:

        DataBrowserModel( FileSystem::Path const& dataDirectoryPath );

        void Initialize( TVector<String> const& fileRestrictions = TVector<String>() );
        void Shutdown();
        void Update( UpdateContext const& context );

        inline TVector<FileSystem::Path> const& GetAllPaths() const { return m_foundPaths; }

        inline FileSystem::Path const& GetSourceDataDirectoryPath() const{ return m_dataDirectoryPath; }
        inline DataDirectoryModel& GetRootDirectory() { return m_rootDirectory; }
        inline DataDirectoryModel const& GetRootDirectory() const { return m_rootDirectory; }

        // Selection
        inline bool IsSelected( FileSystem::Path const& item ) const { return m_selectedItem == item; }
        inline FileSystem::Path const& GetSelection() const { return m_selectedItem; }
        inline void SetSelection( FileSystem::Path const& selectedItem ) { m_selectedItem = selectedItem; }
        inline void ClearSelection() { m_selectedItem = FileSystem::Path(); }

        // Iterator functions
        void ForEachFile( TFunction<void( DataFileModel const& file )> const& function ) const { m_rootDirectory.ForEachFile( function ); }
        void ForEachDirectory( TFunction<void( DataDirectoryModel const& dir )> const& function ) const { m_rootDirectory.ForEachDirectory( function ); }
        void ForEachDirectory( TFunction<void( DataDirectoryModel& dir )> const& function ) { m_rootDirectory.ForEachDirectory( function ); }
        void UpdateFileVisibility( TFunction<bool( DataFileModel const& file )> const& isVisibleFunction ) { m_rootDirectory.UpdateFileVisibility( isVisibleFunction ); }

    private:

        // File system listener
        virtual void OnFileCreated( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnFileDeleted( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnFileRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override { Refresh(); }
        virtual void OnDirectoryCreated( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnDirectoryDeleted( FileSystem::Path const& path ) override { Refresh(); }
        virtual void OnDirectoryRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override { Refresh(); }

        void Refresh();

        DataDirectoryModel& FindOrCreateDirectoryForFile( FileSystem::Path const& path );

    private:

        FileSystem::Path                            m_dataDirectoryPath;
        int32                                       m_dataDirectoryPathDepth;
        FileSystem::FileSystemWatcher               m_fileSystemWatcher;
        TVector<String>                             m_fileRestrictions;

        TVector<FileSystem::Path>                   m_foundPaths;
        DataDirectoryModel                          m_rootDirectory;

        FileSystem::Path                            m_selectedItem;
    };
}