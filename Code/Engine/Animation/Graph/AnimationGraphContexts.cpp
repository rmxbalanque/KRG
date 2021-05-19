#include "AnimationGraphContexts.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphContext::GraphContext()
    {
        #if KRG_DEVELOPMENT_TOOLS
        m_activeNodes.reserve( 50 );
        #endif
    }

    void GraphContext::Initialize( TaskSystem* pTaskSystem, AnimationGraphDataSet const* pDataSet, Pose const* pPreviousPose )
    {
        KRG_ASSERT( m_pPreviousPose == nullptr && pTaskSystem == nullptr && pDataSet == nullptr );
        KRG_ASSERT( pTaskSystem != nullptr && pDataSet != nullptr );

        const_cast<TaskSystem*&>( m_pTaskSystem ) = pTaskSystem;
        const_cast<AnimationGraphDataSet const*&>( m_pDataSet ) = pDataSet;
        const_cast<Pose const*&>( m_pPreviousPose ) = pPreviousPose;

        KRG_ASSERT( m_pPreviousPose->GetSkeleton() == m_pDataSet->GetSkeleton() );

        m_boneMaskPool.Initialize( m_pDataSet->GetSkeleton() );

        //-------------------------------------------------------------------------

        m_deltaTime = 0;
        m_updateID = 0;
        m_worldTransform = Transform::Identity;
        m_worldTransformInverse = Transform::Identity;
        m_sampledEvents.Reset();
        m_branchState = BranchState::Active;
    }

    void GraphContext::Shutdown()
    {
        m_sampledEvents.Reset();
        m_boneMaskPool.Shutdown();

        const_cast<Pose const*&>( m_pPreviousPose ) = nullptr;
        const_cast<AnimationGraphDataSet const*&>( m_pDataSet ) = nullptr;
        const_cast<TaskSystem*&>( m_pTaskSystem ) = nullptr;
    }

    void GraphContext::Update( Seconds const deltaTime, Transform const& prevDisplacementDelta, Transform const& currentWorldTransform )
    {
        m_deltaTime = deltaTime;
        m_updateID++;
        m_sampledEvents.Reset();
        m_worldTransform = currentWorldTransform;
        m_worldTransformInverse = m_worldTransform.Inverse();

        m_branchState = BranchState::Active;
        m_boneMaskPool.Reset();

        KRG_ASSERT( m_pPreviousPose->HasGlobalTransforms() );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_activeNodes.clear();
        m_rootMotionActionRecorder.Reset();
        #endif
    }
}