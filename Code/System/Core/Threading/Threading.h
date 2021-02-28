#pragma once

#include "System/Core/Types/String.h"
#include "System/Core/Time/Time.h"
#include "System/Core/ThirdParty/concurrentqueue/concurrentqueue.h"
#include <mutex>
#include <shared_mutex>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Threading
    {
        using ThreadID = U32;

        //-------------------------------------------------------------------------
        // Processor Info
        //-------------------------------------------------------------------------

        struct ProcessorInfo
        {
            U16 m_numPhysicalCores = 0;
            U16 m_numLogicalCores = 0;
        };

        KRG_SYSTEM_CORE_API ProcessorInfo GetProcessorInfo();

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

        // Read/Write lock
        class ReadWriteMutex
        {
        public:

            inline void LockForWrite() { m_mutex.lock(); }
            inline bool TryLockForWrite() { return m_mutex.try_lock(); }

            inline void LockForRead() { m_mutex.lock_shared(); }
            inline bool TryLockForRead() { return m_mutex.try_lock_shared(); }

        private:

            std::shared_mutex m_mutex;
        };

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
        // Data structures
        //-------------------------------------------------------------------------

        template<typename T, typename Traits = moodycamel::ConcurrentQueueDefaultTraits> using LockFreeQueue = moodycamel::ConcurrentQueue<T, Traits>;

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