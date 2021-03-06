#include "TaskSystem.h"
#include "Threading.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static void OnStartThread( uint32 threadNum )
    {
        Memory::InitializeThreadHeap();

        char nameBuffer[100];
        Printf( nameBuffer, 100, "KRG Worker %d", threadNum );

        KRG_PROFILE_THREAD_START( nameBuffer );
        Threading::SetCurrentThreadName( nameBuffer );
    }

    static void OnStopThread( uint32 threadNum )
    {
        Memory::ShutdownThreadHeap();
        KRG_PROFILE_THREAD_END();
    }

    static void* CustomAllocFunc( size_t alignment, size_t size, void* userData_, const char* file_, int line_ )
    {
        return KRG::Alloc( size, alignment );
    }

    static void CustomFreeFunc( void* ptr, size_t size, void* userData_, const char* file_, int line_ )
    {
        KRG::Free( ptr );
    }

    //-------------------------------------------------------------------------

    TaskSystem::TaskSystem()
    {
        // Get number of worker threads that we should create (excluding main thread)
        auto const processorInfo = Threading::GetProcessorInfo();
        const_cast<uint32&>( m_numWorkers ) = processorInfo.m_numPhysicalCores - 1;
        KRG_ASSERT( m_numWorkers >= 0 );
    }

    TaskSystem::~TaskSystem()
    {
        KRG_ASSERT( !m_initialized );
    }

    void TaskSystem::Initialize()
    {
        enki::TaskSchedulerConfig config;
        config.numTaskThreadsToCreate = m_numWorkers;
        config.customAllocator.alloc = CustomAllocFunc;
        config.customAllocator.free = CustomFreeFunc;
        config.profilerCallbacks.threadStart = OnStartThread;
        config.profilerCallbacks.threadStop = OnStopThread;

        m_taskScheduler.Initialize( config );
        m_initialized = true;
    }

    void TaskSystem::Shutdown()
    {
        m_taskScheduler.WaitforAllAndShutdown();
        m_initialized = false;
    }
}
