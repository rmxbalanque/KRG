#include "DataBrowserModel.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    DataBrowserModel::DataBrowserModel( FileSystem::Path const& dataDirectoryPath )
        : m_dataDirectoryPath( dataDirectoryPath )
        , m_rootDirectory( "Data", dataDirectoryPath )
    {
        KRG_ASSERT( m_dataDirectoryPath.ExistsAndIsDirectory() );
        m_dataDirectoryPathDepth = m_dataDirectoryPath.GetPathDepth();
    }

    void DataBrowserModel::Initialize( TVector<String> const& fileRestrictions )
    {
        m_fileRestrictions = fileRestrictions;
        Refresh();
        m_fileSystemWatcher.StartWatching( m_dataDirectoryPath );
    }

    void DataBrowserModel::Shutdown()
    {
        m_fileSystemWatcher.StopWatching();
        m_rootDirectory = DataDirectoryModel();
    }

    void DataBrowserModel::Update( UpdateContext const& context )
    {
        m_fileSystemWatcher.Update();
    }

    void DataBrowserModel::Refresh()
    {
        // OPTIMIZE THIS!

        //-------------------------------------------------------------------------

        m_rootDirectory.m_directories.clear();
        m_rootDirectory.m_files.clear();
        m_rootDirectory.SetExpanded( true );

        FileSystem::GetDirectoryContents( m_dataDirectoryPath, m_foundPaths, FileSystem::DirectoryReaderOutput::OnlyFiles, FileSystem::DirectoryReaderMode::Expand, m_fileRestrictions );

        //-------------------------------------------------------------------------

        for ( auto const& path : m_foundPaths )
        {
            auto& directory = FindOrCreateDirectoryForFile( path );
            directory.m_files.emplace_back( DataFileModel( path ) );
        }
    }

    //-------------------------------------------------------------------------

    DataDirectoryModel& DataBrowserModel::FindOrCreateDirectoryForFile( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsFilePath() );

        DataDirectoryModel* pFoundDirectory = &m_rootDirectory;

        TInlineVector<String, 10> splitPath = path.Split();

        //-------------------------------------------------------------------------

        FileSystem::Path directoryPath = m_rootDirectory.GetPath();
        int32 const pathDepth = (int32) splitPath.size();
        for ( int32 i = m_dataDirectoryPathDepth + 1; i < pathDepth; i++ )
        {
            directoryPath.Append( splitPath[i] );

            auto dirIter = VectorFind( pFoundDirectory->m_directories, splitPath[i], [] ( DataDirectoryModel const& dir, String const& name ) { return dir.m_name == name; } );
            if ( dirIter == pFoundDirectory->m_directories.end() )
            {
                auto& directory = pFoundDirectory->m_directories.emplace_back( DataDirectoryModel( splitPath[i].c_str(), directoryPath ) );
                pFoundDirectory = &directory;
            }
            else
            {
                pFoundDirectory = &*dirIter;
            }
        }

        //-------------------------------------------------------------------------

        return *pFoundDirectory;
    }
}