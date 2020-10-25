#include "FileSystemWatcher.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Platform/Platform/Platform_Win32.h"

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

#if _WIN32
namespace KRG
{
    namespace FileSystem
    {
        FileSystemWatcher::~FileSystemWatcher()
        {
            KRG_ASSERT( m_changeListeners.empty() );
            StopWatching();
        }

        void FileSystemWatcher::RegisterChangeListener( IFileSystemChangeListener* pListener )
        {
            KRG_ASSERT( pListener != nullptr && !VectorContains( m_changeListeners, pListener ) );
            m_changeListeners.emplace_back( pListener );
        }

        void FileSystemWatcher::UnregisterChangeListener( IFileSystemChangeListener* pListener )
        {
            KRG_ASSERT( pListener != nullptr && VectorContains( m_changeListeners, pListener ) );
            m_changeListeners.erase_first_unsorted( pListener );
        }

        bool FileSystemWatcher::StartWatching( FileSystemPath const& directoryToWatch )
        {
            KRG_ASSERT( !IsWatching() );
            KRG_ASSERT( directoryToWatch.IsValid() && directoryToWatch.IsDirectoryPath() );

            // Get directory handle
            //-------------------------------------------------------------------------

            m_pDirectoryHandle = CreateFile( directoryToWatch.c_str(), GENERIC_READ | FILE_LIST_DIRECTORY,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                            NULL );

            if ( m_pDirectoryHandle == INVALID_HANDLE_VALUE )
            {
                KRG_LOG_ERROR( "FileSystem", "Failed to open handle to directory (%s), error: %s", m_directoryToWatch.c_str(), Platform::Win32::GetLastErrorMessage().c_str() );
                m_pDirectoryHandle = nullptr;
                return false;
            }

            // Create event
            //-------------------------------------------------------------------------

            m_overlappedEvent.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
            if ( m_overlappedEvent.hEvent == nullptr )
            {
                KRG_LOG_ERROR( "FileSystem", "Failed to create overlapped event: %s", Platform::Win32::GetLastErrorMessage().c_str() );
                CloseHandle( m_pDirectoryHandle );
                m_pDirectoryHandle = nullptr;
                return false;
            }

            m_directoryToWatch = directoryToWatch;
            return true;
        }

        void FileSystemWatcher::StopWatching()
        {
            // If we are still waiting for an IO request, cancel it
            if( m_requestPending )
            {
                CancelIo( m_pDirectoryHandle );
                GetOverlappedResult( m_pDirectoryHandle, &m_overlappedEvent, &m_numBytesReturned, TRUE );
            }

            // Send all pending notifications
            for( auto& event : m_pendingFileModificationEvents )
            {
                for ( auto pChangeHandler : m_changeListeners )
                {
                    pChangeHandler->OnFileModified( event.m_path );
                }
            }

            m_pendingFileModificationEvents.clear();

            //-------------------------------------------------------------------------

            CloseHandle( m_overlappedEvent.hEvent );
            CloseHandle( m_pDirectoryHandle );
            m_directoryToWatch = FileSystemPath();
            m_overlappedEvent.hEvent = nullptr;
            m_pDirectoryHandle = nullptr;
        }

        void FileSystemWatcher::Update()
        {
            KRG_ASSERT( IsWatching() );

            if( !m_requestPending )
            {
                m_requestPending = ReadDirectoryChangesW
                (
                    m_pDirectoryHandle,
                    m_resultBuffer,
                    ResultBufferSize,
                    TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                    &m_numBytesReturned,
                    &m_overlappedEvent,
                    NULL
                );

                KRG_ASSERT( m_requestPending );
            }
            else // Wait for request to complete
            {
                if ( GetOverlappedResult( m_pDirectoryHandle, &m_overlappedEvent, &m_numBytesReturned, FALSE ) )
                {
                    if ( m_numBytesReturned != 0 )
                    {
                        ProcessResults();
                    }

                    m_requestPending = false;
                }
                else // Error occurred or request not complete
                {
                    if ( GetLastError() != ERROR_IO_INCOMPLETE )
                    {
                        KRG_LOG_FATAL_ERROR( "FileSystem", "FileSystemWatcher failed to get overlapped results: %s", Platform::Win32::GetLastErrorMessage().c_str() );
                        KRG_HALT();
                    }
                }
            }

            //-------------------------------------------------------------------------

            ProcessPendingModificationEvents();
        }

        //-------------------------------------------------------------------------

        namespace
        {
            static FileSystemPath GetFileSystemPath( FileSystemPath const& dirPath, FILE_NOTIFY_INFORMATION* pNotifyInformation )
            {
                KRG_ASSERT( pNotifyInformation != nullptr );

                char strBuffer[256] = { 0 };
                wcstombs( strBuffer, pNotifyInformation->FileName, pNotifyInformation->FileNameLength );

                FileSystemPath filePath = dirPath;
                filePath.Append( strBuffer );
                return filePath;
            }
        }

        void FileSystemWatcher::ProcessResults()
        {
            FileSystemPath path, secondPath;

            FILE_NOTIFY_INFORMATION* pNotify = nullptr;
            size_t offset = 0;

            do
            {
                pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>( m_resultBuffer + offset );

                switch ( pNotify->Action )
                {
                    case FILE_ACTION_ADDED:
                    {
                        path = GetFileSystemPath( m_directoryToWatch, pNotify );

                        if ( path.IsDirectoryPath() )
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnDirectoryCreated( path );
                            }
                        }
                        else
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnFileCreated( path );
                            }
                        }
                    }
                    break;

                    case FILE_ACTION_REMOVED:
                    {
                        path = GetFileSystemPath( m_directoryToWatch, pNotify );

                        if ( path.IsDirectoryPath() )
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnDirectoryDeleted( path );
                            }
                        }
                        else
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnFileDeleted( path );
                            }
                        }
                    }
                    break;

                    case FILE_ACTION_MODIFIED:
                    {
                        path = GetFileSystemPath( m_directoryToWatch, pNotify );
                        if ( path.IsFilePath() )
                        {
                            auto predicate = [] ( FileModificationEvent const& event, FileSystemPath const& path )
                            {
                                return event.m_path == path;
                            };

                            if ( !VectorContains( m_pendingFileModificationEvents, path, predicate ) )
                            {
                                m_pendingFileModificationEvents.emplace_back( FileModificationEvent( path ) );
                            }
                        }
                    }
                    break;

                    case FILE_ACTION_RENAMED_OLD_NAME:
                    {
                        // Get old name
                        path = GetFileSystemPath( m_directoryToWatch, pNotify );

                        // Get new name
                        KRG_ASSERT( pNotify->NextEntryOffset != 0 );
                        offset += pNotify->NextEntryOffset;
                        pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>( m_resultBuffer + offset );
                        KRG_ASSERT( pNotify->Action == FILE_ACTION_RENAMED_NEW_NAME );

                        secondPath = GetFileSystemPath( m_directoryToWatch, pNotify );

                        //-------------------------------------------------------------------------

                        if ( path.IsDirectoryPath() )
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnDirectoryRenamed( path, secondPath );
                            }
                        }
                        else
                        {
                            for ( auto pChangeHandler : m_changeListeners )
                            {
                                pChangeHandler->OnFileRenamed( path, secondPath );
                            }
                        }
                    }
                    break;
                }

                offset += pNotify->NextEntryOffset;

            } while ( pNotify->NextEntryOffset != 0 );

            // Clear the result buffer
            Memory::MemsetZero( m_resultBuffer, ResultBufferSize );
        }

        void FileSystemWatcher::ProcessPendingModificationEvents()
        {
            for ( S32 i = (S32) m_pendingFileModificationEvents.size() - 1; i >= 0; i-- )
            {
                auto& pendingEvent = m_pendingFileModificationEvents[i];

                Milliseconds const elapsedTime = Milliseconds::Now() - pendingEvent.m_startTime;
                if( elapsedTime > FileModificationBatchTimeout )
                {
                    for ( auto pChangeHandler : m_changeListeners )
                    {
                        pChangeHandler->OnFileModified( pendingEvent.m_path );
                    }

                    VectorEraseUnsorted( m_pendingFileModificationEvents, i );
                }
            }
        }
    }
}
#endif