#include "AnimationGraphComponent.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationGraphComponent::Initialize()
    {
        AnimationComponent::Initialize();
        KRG_ASSERT( m_pGraph != nullptr && m_pDataSet.IsLoaded() );

        m_pPose = KRG::New<Pose>( m_pDataSet->GetSkeleton() );
        m_pPose->CalculateGlobalTransforms();

        m_pTaskSystem = KRG::New<Graph::TaskSystem>( m_pDataSet->GetSkeleton() );
        m_graphContext.Initialize( m_pTaskSystem, m_pDataSet.GetPtr(), m_pPose );

        m_pGraphInstance = KRG::New<AnimationGraphInstance>( m_pGraph.GetPtr() );
        m_pGraphInstance->Initialize( m_graphContext );
    }

    void AnimationGraphComponent::Shutdown()
    {
        m_pGraphInstance->Shutdown( m_graphContext );
        m_graphContext.Shutdown();

        KRG::Delete( m_pTaskSystem );
        KRG::Delete( m_pGraphInstance );
        KRG::Delete( m_pPose );

        AnimationComponent::Shutdown();
    }

    //-------------------------------------------------------------------------

    Skeleton const* AnimationGraphComponent::GetSkeleton() const
    {
        KRG_ASSERT( m_pDataSet != nullptr );
        return m_pDataSet->GetSkeleton();
    }

    void AnimationGraphComponent::Update( UpdateContext const& ctx )
    {
        if ( ctx.GetUpdateStage() == UpdateStage::PrePhysics )
        {
            m_graphContext.Update( ctx.GetDeltaTime(), Transform::Identity, Transform::Identity );
            m_pGraphInstance->UpdateGraph( m_graphContext );
            m_pTaskSystem->UpdatePrePhysics( m_graphContext.m_deltaTime, m_graphContext.m_worldTransform, m_graphContext.m_worldTransformInverse );
        }
        else if ( ctx.GetUpdateStage() == UpdateStage::PostPhysics )
        {
            m_pTaskSystem->UpdatePostPhysics( *m_pPose );
        }
    }
}