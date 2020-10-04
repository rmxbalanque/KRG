#pragma once

#include "System/Core/Types/String.h"
#include "System/Core/Types/Time.h"
#include <mutex>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Threading
    {
        using ThreadID = U32;

        //-------------------------------------------------------------------------
        // Mutexes and Locks
        //-------------------------------------------------------------------------

        typedef std::mutex Mutex;
        typedef std::recursive_mutex RecursiveMutex;
        typedef std::condition_variable ConditionVariable;

        using Lock = std::unique_lock<Mutex>;
        using RecursiveLock = std::unique_lock<RecursiveMutex>;

        using ScopeLock = std::lock_guard<Mutex>;
        using RecursiveScopeLock = std::lock_guard<RecursiveMutex>;

        //-------------------------------------------------------------------------
        // Synchronization Event Semaphore
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_CORE_API SyncEvent
        {
        public:

            SyncEvent();

            void Signal();
            void Reset();

            void Wait() const;
            void Wait( Milliseconds maxWaitTime ) const;

        private:

            void* m_pNativeHandle;
        };

        //-------------------------------------------------------------------------
        // Utility Functions
        //-------------------------------------------------------------------------

        inline void Sleep( Milliseconds time )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( time ) );
        }

        KRG_SYSTEM_CORE_API bool IsMainThread();

        //-------------------------------------------------------------------------
        // Global State
        //-------------------------------------------------------------------------

        KRG_SYSTEM_CORE_API void Initialize( char const* pMainThreadName );
        KRG_SYSTEM_CORE_API void Shutdown();
        KRG_SYSTEM_CORE_API ThreadID GetCurrentThreadID();
        KRG_SYSTEM_CORE_API void SetCurrentThreadName( char const* pName );
    }
}