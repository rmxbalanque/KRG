#include "DataDirectoryModel.h"
#include "System/Core/Settings/ConfigSettings.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        static ConfigSettingString g_sourceDataPath( "SourceDataPath", "Paths", "" );
        static ConfigSettingString g_compiledDataPath( "CompiledDataPath", "Paths", "" );
    }

    //-------------------------------------------------------------------------

    DataDirectoryModel::DataDirectoryModel()
        : m_dataDirectoryPath( FileSystem::Path( Settings::g_sourceDataPath.GetValue() ) )
        , m_rootDirectory( "Data", m_dataDirectoryPath )
    {
        KRG_ASSERT( m_dataDirectoryPath.ExistsAndIsDirectory() );
        m_dataDirectoryPathDepth = m_dataDirectoryPath.GetPathDepth();
        m_rootDirectory.m_isExpanded = true;
    }

    void DataDirectoryModel::Initialize( char const* const filter[] )
    {
        FileSystem::GetDirectoryContents( m_dataDirectoryPath, m_foundPaths, FileSystem::DirectoryReaderOutput::OnlyFiles, FileSystem::DirectoryReaderMode::Expand, filter );

        //-------------------------------------------------------------------------

        for ( auto const& path : m_foundPaths )
        {
            auto& directory = FindOrCreateDirectoryForFile( path );
            directory.m_files.insert( File( path ) );
        }

        //-------------------------------------------------------------------------
        
        m_fileSystemWatcher.StartWatching( m_dataDirectoryPath );
    }

    void DataDirectoryModel::Shutdown()
    {
        m_fileSystemWatcher.StopWatching();
        m_rootDirectory = Directory();
    }

    void DataDirectoryModel::Update( UpdateContext const& context )
    {
        m_fileSystemWatcher.Update();
    }

    //-------------------------------------------------------------------------

    DataDirectoryModel::Directory& DataDirectoryModel::FindOrCreateDirectoryForFile( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsFilePath() );

        Directory* pFoundDirectory = &m_rootDirectory;

        TInlineVector<String, 10> splitPath = path.Split();

        //-------------------------------------------------------------------------

        int32 const pathDepth = (int32) splitPath.size();
        for ( int32 i = m_dataDirectoryPathDepth + 1; i < pathDepth; i++ )
        {
            auto dirIter = pFoundDirectory->m_directories.find_as( splitPath[i] );
            if ( dirIter == pFoundDirectory->m_directories.end() )
            {
                auto addedDir = pFoundDirectory->m_directories.insert( Directory( splitPath[i].c_str(), path ) );
                pFoundDirectory = const_cast<Directory*>( &*addedDir.first ); 
            }
            else
            {
                pFoundDirectory = const_cast<Directory*>( &*dirIter );
            }
        }

        //-------------------------------------------------------------------------

        return *pFoundDirectory;
    }
}