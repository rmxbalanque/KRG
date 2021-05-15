#pragma once

#include "AnimGraphTask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class TaskSystem
    {

    public:

        typedef int8 TaskMarker;

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
        inline int8 RegisterTask( ConstructorParams&&... params )
        {
            auto pNewTask = m_tasks.emplace_back( KRG::New<T>( std::forward<ConstructorParams>( params )... ) );
            m_hasPhysicsDependency |= pNewTask->HasPhysicsDependency();
            return (int8) ( m_tasks.size() - 1 );
        }

        TaskMarker GetCurrentMarker() const { return (int8) m_tasks.size(); }
        void RollbackToMarker( TaskMarker const marker );

    private:

        bool AddTaskChainToPrePhysicsList( int8 taskIdx );
        void ExecuteTasks();

    private:

        TVector<Task*>                  m_tasks;
        PoseBufferPool                  m_posePool;
        TaskContext                     m_taskContext;
        TInlineVector<int8, 16>         m_prePhysicsTaskIndices;
        bool                            m_hasPhysicsDependency = false;
        bool                            m_hasCodependentPhysicsTasks = false;
    };
}