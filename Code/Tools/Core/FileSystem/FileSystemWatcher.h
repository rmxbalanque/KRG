#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------
// Basic Win32 File System Watcher
//-------------------------------------------------------------------------
// Implementation will try to batch file modification notification to prevent sending multiple events for the same operation
// The OS level function (ReadDirectoryChangesW) will trigger a modification event for multiple operations that are part of a logical operation
//-------------------------------------------------------------------------

#if _WIN32
namespace KRG::FileSystem
{
    class KRG_TOOLS_CORE_API IFileSystemChangeListener
    {
    public:

        virtual void OnFileCreated( FileSystem::Path const& path ) {};
        virtual void OnFileDeleted( FileSystem::Path const& path ) {};
        virtual void OnFileRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) {};

        virtual void OnFileModified( FileSystem::Path const& path ) {};

        virtual void OnDirectoryCreated( FileSystem::Path const& path ) {};
        virtual void OnDirectoryDeleted( FileSystem::Path const& path ) {};
        virtual void OnDirectoryRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) {};
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API FileSystemWatcher final
    {
        static constexpr uint32 const ResultBufferSize = 16384;
        static constexpr float const FileModificationBatchTimeout = 250; // How long to wait between the start of the first modification event and notifying the event listeners

        struct FileModificationEvent
        {
            FileModificationEvent( FileSystem::Path const& path ) : m_path( path ) { KRG_ASSERT( path.IsValid() && path.IsFile() ); }

            FileSystem::Path                            m_path;
            Milliseconds                                m_startTime = SystemClock::GetTimeInMilliseconds();
        };

    public:

        ~FileSystemWatcher();

        void RegisterChangeListener( IFileSystemChangeListener* pListener );
        void UnregisterChangeListener( IFileSystemChangeListener* pListener );

        bool StartWatching( FileSystem::Path const& directoryToWatch );
        bool IsWatching() const { return m_pDirectoryHandle != nullptr; }
        void StopWatching();

        // Returns true if any filesystem changes detected!
        bool Update();

    private:

        void ProcessResults();
        void ProcessPendingModificationEvents();

    private:

        FileSystem::Path                                m_directoryToWatch;
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
#endif