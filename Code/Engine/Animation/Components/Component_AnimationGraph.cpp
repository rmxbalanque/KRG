#include "Component_AnimationGraph.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationGraphComponent::Initialize()
    {
        KRG_ASSERT( m_pGraphVariation != nullptr && m_pGraphVariation.IsLoaded() );
        AnimationComponent::Initialize();

        m_pPose = KRG::New<Pose>( m_pGraphVariation->GetSkeleton() );
        m_pPose->CalculateGlobalTransforms();

        m_pTaskSystem = KRG::New<Graph::TaskSystem>( m_pGraphVariation->GetSkeleton() );
        m_graphContext.Initialize( m_pTaskSystem, m_pPose );

        m_pGraphInstance = KRG::New<Graph::GraphInstance>( m_pGraphVariation.GetPtr() );
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

    void AnimationGraphComponent::SetGraphVariation( ResourceID graphResourceID )
    {
        KRG_ASSERT( IsUnloaded() );

        KRG_ASSERT( graphResourceID.IsValid() );
        m_pGraphVariation = graphResourceID;
    }

    //-------------------------------------------------------------------------

    Skeleton const* AnimationGraphComponent::GetSkeleton() const
    {
        KRG_ASSERT( m_pGraphVariation != nullptr );
        return m_pGraphVariation->GetSkeleton();
    }

    void AnimationGraphComponent::PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        m_graphContext.Update( deltaTime, characterTransform );

        m_pTaskSystem->Reset();
        Graph::PoseNodeResult const result = m_pGraphInstance->UpdateGraph( m_graphContext );
        m_rootMotionDelta = result.m_rootMotionDelta;
        m_pTaskSystem->UpdatePrePhysics( m_graphContext.m_deltaTime, m_graphContext.m_worldTransform, m_graphContext.m_worldTransformInverse );
    }

    void AnimationGraphComponent::PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        m_pTaskSystem->UpdatePostPhysics( *m_pPose );
    }
}