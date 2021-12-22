#include "Animation_RuntimeGraph_TaskSystem.h"
#include "Tasks/Animation_RuntimeGraphTask_DefaultPose.h"
#include "Engine/Animation/AnimationBlender.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    TaskSystem::TaskSystem( Skeleton const* pSkeleton )
        : m_posePool( pSkeleton )
        , m_taskContext( m_posePool )
    {
        KRG_ASSERT( pSkeleton != nullptr );
    }

    TaskSystem::~TaskSystem()
    {
        Reset();
    }

    void TaskSystem::Reset()
    {
        for ( auto pTask : m_tasks )
        {
            KRG::Delete( pTask );
        }

        m_tasks.clear();
        m_posePool.Reset();
        m_hasPhysicsDependency = false;
    }

    //-------------------------------------------------------------------------

    void TaskSystem::RollbackToTaskIndexMarker( TaskIndex const marker )
    {
        KRG_ASSERT( marker >= 0 && marker <= m_tasks.size() );

        for ( int16 t = (int16) m_tasks.size() - 1; t >= marker; t-- )
        {
            KRG::Delete( m_tasks[t] );
            m_tasks.erase( m_tasks.begin() + t );
        }
    }

    //-------------------------------------------------------------------------

    bool TaskSystem::AddTaskChainToPrePhysicsList( TaskIndex taskIdx )
    {
        KRG_ASSERT( taskIdx >= 0 && taskIdx < m_tasks.size() );
        KRG_ASSERT( m_hasCodependentPhysicsTasks == false );

        auto pTask = m_tasks[taskIdx];
        for ( auto DepTaskIdx : pTask->GetDependencyIndices() )
        {
            if ( !AddTaskChainToPrePhysicsList( DepTaskIdx ) )
            {
                return false;
            }
        }

        //-------------------------------------------------------------------------

        // Cant have a dependency that relies on physics
        if ( pTask->GetRequiredUpdateStage() == Task::UpdateStage::PostPhysics )
        {
            return false;
        }

        // Cant add the same task twice, i.e. codependency
        if ( VectorContains( m_prePhysicsTaskIndices, taskIdx ) )
        {
            return false;
        }

        m_prePhysicsTaskIndices.emplace_back( taskIdx );
        return true;
    }

    void TaskSystem::UpdatePrePhysics( float deltaTime, Transform const& worldTransform, Transform const& worldTransformInverse )
    {
        m_taskContext.m_deltaTime = deltaTime;
        m_taskContext.m_worldTransform = worldTransform;
        m_taskContext.m_worldTransformInverse = worldTransformInverse;

        m_prePhysicsTaskIndices.clear();
        m_hasCodependentPhysicsTasks = false;

        // Conditionally execute all pre-physics tasks
        //-------------------------------------------------------------------------

        if ( m_hasPhysicsDependency )
        {
            // Go backwards through the registered task and execute all task chains with a pre-physics requirement
            auto const numTasks = (int8) m_tasks.size();
            for ( TaskIndex i = 0; i < numTasks; i++ )
            {
                if ( m_tasks[i]->GetRequiredUpdateStage() == Task::UpdateStage::PrePhysics )
                {
                    if ( !AddTaskChainToPrePhysicsList( i ) )
                    {
                        m_hasCodependentPhysicsTasks = true;
                        break;
                    }
                }
            }

            // If we've detected a co-dependent physics task, ignore all registered tasks by just pushing a new task and immediately executing it
            if ( m_hasCodependentPhysicsTasks )
            {
                KRG_LOG_WARNING( "Animation", "Co-dependent physics tasks detected!" );
                RegisterTask<DefaultPoseTask>( (int16) InvalidIndex, Pose::InitialState::ReferencePose );
                m_tasks.back()->Execute( m_taskContext );
            }
            else // Execute pre-physics tasks
            {
                for ( TaskIndex prePhysicsTaskIdx : m_prePhysicsTaskIndices )
                {
                    // Set dependencies
                    m_taskContext.m_dependencies.clear();
                    for ( auto depTaskIdx : m_tasks[prePhysicsTaskIdx]->GetDependencyIndices() )
                    {
                        KRG_ASSERT( m_tasks[depTaskIdx]->IsComplete() );
                        m_taskContext.m_dependencies.emplace_back( m_tasks[depTaskIdx] );
                    }

                    m_tasks[prePhysicsTaskIdx]->Execute( m_taskContext );
                }
            }
        }
        else // If we have no physics dependent tasks, execute all tasks now
        {
            ExecuteTasks();
        }
    }

    void TaskSystem::UpdatePostPhysics( Pose& outPose )
    {
        // If we detected co-dependent tasks in the pre-physics update, there's nothing to do here
        if ( m_hasCodependentPhysicsTasks )
        {
            return;
        }

        // Execute tasks
        //-------------------------------------------------------------------------
        // Only run tasks if we have a physics dependency, else all tasks were already executed in the first update stage

        if ( m_hasPhysicsDependency )
        {
            ExecuteTasks();
        }

        // Reflect animation pose out
        //-------------------------------------------------------------------------

        if ( !m_tasks.empty() )
        {
            auto pFinalTask = m_tasks.back();
            KRG_ASSERT( pFinalTask->IsComplete() );
            PoseBuffer const* pResultPoseBuffer = m_posePool.GetBuffer( pFinalTask->GetResultBufferIndex() );
            Pose const* pResultPose = &pResultPoseBuffer->m_pose;

            // Always return a non-additive pose
            if ( pResultPose->IsAdditivePose() )
            {
                outPose.Reset( Pose::InitialState::ReferencePose );
                TBitFlags<PoseBlendOptions> blendOptions( PoseBlendOptions::Additive );
                Blender::Blend( &outPose, pResultPose, 1.0f, blendOptions, nullptr, &outPose );
            }
            else // Just copy the pose
            {
                outPose.CopyFrom( pResultPoseBuffer->m_pose );
            }

            // Calculate the global transforms and release the task pose buffer
            outPose.CalculateGlobalTransforms();
            m_posePool.ReleasePoseBuffer( pFinalTask->GetResultBufferIndex() );
        }
        else
        {
            outPose.Reset( Pose::InitialState::ReferencePose, true );
        }
    }

    void TaskSystem::ExecuteTasks()
    {
        int16 const numTasks = (int8) m_tasks.size();
        for ( TaskIndex i = 0; i < numTasks; i++ )
        {
            if ( !m_tasks[i]->IsComplete() )
            {
                // Set dependencies
                m_taskContext.m_dependencies.clear();
                for ( auto DepTaskIdx : m_tasks[i]->GetDependencyIndices() )
                {
                    KRG_ASSERT( m_tasks[DepTaskIdx]->IsComplete() );
                    m_taskContext.m_dependencies.emplace_back( m_tasks[DepTaskIdx] );
                }

                // Execute task
                m_tasks[i]->Execute( m_taskContext );
            }
        }
    }
}