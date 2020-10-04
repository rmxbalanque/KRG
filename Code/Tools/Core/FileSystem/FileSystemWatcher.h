#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------
// Basic Win32 File System Watcher
//-------------------------------------------------------------------------
// Implementation will try to batch file modification notification to prevent sending multiple events for the same operation
// The OS level function (ReadDirectoryChangesW) will trigger a modification event for multiple operations that are part of a logical operation
//-------------------------------------------------------------------------

#if _WIN32
namespace KRG
{
    namespace FileSystem
    {
        class IFileSystemChangeListener
        {
        public:

            virtual void OnFileCreated( FileSystemPath const& path ) {};
            virtual void OnFileDeleted( FileSystemPath const& path ) {};
            virtual void OnFileRenamed( FileSystemPath const& oldPath, FileSystemPath const& newPath ) {};

            virtual void OnFileModified( FileSystemPath const& path ) {};

            virtual void OnDirectoryCreated( FileSystemPath const& path ) {};
            virtual void OnDirectoryDeleted( FileSystemPath const& path ) {};
            virtual void OnDirectoryRenamed( FileSystemPath const& oldPath, FileSystemPath const& newPath ) {};
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API FileSystemWatcher
        {
            static constexpr U32 const ResultBufferSize = 16384; // 1000 results
            static constexpr F32 const FileModificationBatchTimeout = 250; // How long to wait between the start of the first modification event and notifying the event listeners

            struct FileModificationEvent
            {
                FileModificationEvent( FileSystemPath const& path ) : m_path( path ) { KRG_ASSERT( path.IsValid() && path.IsFilePath() ); }

                FileSystemPath      m_path;
                Milliseconds        m_startTime = Milliseconds::Now();
            };

        public:

            ~FileSystemWatcher();

            void RegisterChangeListener( IFileSystemChangeListener* pListener );
            void UnregisterChangeListener( IFileSystemChangeListener* pListener );

            bool StartWatching( FileSystemPath const& directoryToWatch );
            bool IsWatching() const { return m_pDirectoryHandle != nullptr; }
            void StopWatching();

            void Update();

        private:

            void ProcessResults();
            void ProcessPendingModificationEvents();

        private:

            FileSystemPath                                  m_directoryToWatch;
            void*                                           m_pDirectoryHandle = nullptr;

            // Listeners
            TInlineVector<IFileSystemChangeListener*, 5>    m_changeListeners;

            // Request Data
            OVERLAPPED                                      m_overlappedEvent = { 0 };
            Byte                                            m_resultBuffer[ResultBufferSize] = { 0 };
            DWORD                                           m_numBytesReturned = 0;
            bool                                            m_requestPending = false;

            // File Modification buffers
            TVector<FileModificationEvent>                  m_pendingFileModificationEvents;
        };
    }
}
#endif