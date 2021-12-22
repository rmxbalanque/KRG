#pragma once

#include "Animation_RuntimeGraph_Task.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class TaskSystem
    {

    public:

        TaskSystem( Skeleton const* pSkeleton );
        ~TaskSystem();

        void Reset();

        // Execution
        //-------------------------------------------------------------------------

        inline bool HasPhysicsDependency() const { return m_hasPhysicsDependency; }
        void UpdatePrePhysics( float deltaTime, Transform const& worldTransform, Transform const& worldTransformInverse );
        void UpdatePostPhysics( Pose& outPose );

        // Cached Pose storage
        //-------------------------------------------------------------------------

        inline UUID CreateCachedPose() { return m_posePool.CreateCachedPoseBuffer(); }
        inline void DestroyCachedPose( UUID const& cachedPoseID ) { m_posePool.DestroyCachedPoseBuffer( cachedPoseID ); }

        // Task Registration
        //-------------------------------------------------------------------------

        inline bool HasTasks() const { return m_tasks.size() > 0; }
        inline TVector<Task*> const& GetRegisteredTasks() const { return m_tasks; }

        template< typename T, typename ... ConstructorParams >
        inline TaskIndex RegisterTask( ConstructorParams&&... params )
        {
            KRG_ASSERT( m_tasks.size() < 0xFF );
            auto pNewTask = m_tasks.emplace_back( KRG::New<T>( std::forward<ConstructorParams>( params )... ) );
            m_hasPhysicsDependency |= pNewTask->HasPhysicsDependency();
            return (TaskIndex) ( m_tasks.size() - 1 );
        }

        TaskIndex GetCurrentTaskIndexMarker() const { return (TaskIndex) m_tasks.size(); }
        void RollbackToTaskIndexMarker( TaskIndex const marker );

    private:

        bool AddTaskChainToPrePhysicsList( TaskIndex taskIdx );
        void ExecuteTasks();

    private:

        TVector<Task*>                  m_tasks;
        PoseBufferPool                  m_posePool;
        TaskContext                     m_taskContext;
        TInlineVector<TaskIndex, 16>    m_prePhysicsTaskIndices;
        bool                            m_hasPhysicsDependency = false;
        bool                            m_hasCodependentPhysicsTasks = false;
    };
}