#pragma once

#include "../_Module/API.h"
#include "System/Core/ThirdParty/EnkiTS/TaskScheduler.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Task scheduler
    //-------------------------------------------------------------------------
    // * Wrapper on top of EnkiTS
    //-------------------------------------------------------------------------
    // Note: Will also use main thread as a worker thread - when using blocking wait!

    using IAsyncTask = enki::ITaskSet;
    using AsyncTask = enki::TaskSet;
    using TaskSetPartition = enki::TaskSetPartition;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API TaskSystem : public ISystem
    {

    public:

        KRG_SYSTEM_ID( TaskSystem );

    public:

        TaskSystem();
        ~TaskSystem();

        inline bool IsInitialized() const { return m_initialized; }
        void Initialize();
        void Shutdown();

        inline uint32 GetNumWorkers() const { return m_numWorkers; }

        inline void WaitForAll() { m_taskScheduler.WaitforAll(); }

        inline void ScheduleTask( IAsyncTask* pTask )
        {
            KRG_ASSERT( m_initialized );
            m_taskScheduler.AddTaskSetToPipe( pTask );
        }

        inline void WaitForTask( IAsyncTask* pTask )
        {
            m_taskScheduler.WaitforTask( pTask );
        }

    private:

        enki::TaskScheduler     m_taskScheduler;
        uint32                  m_numWorkers = 0;
        bool                    m_initialized = false;
    };
}