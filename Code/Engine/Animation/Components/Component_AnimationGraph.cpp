#include "Component_AnimationGraph.h"
#include "System/Animation/AnimationPose.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphComponent::~AnimationGraphComponent()
    {
        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( m_pRootMotionActionRecorder == nullptr );
        #endif
    }

    void AnimationGraphComponent::Initialize()
    {
        KRG_ASSERT( m_pGraphVariation != nullptr && m_pGraphVariation.IsLoaded() );
        EntityComponent::Initialize();

        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder = KRG::New<RootMotionRecorder>();
        #endif

        m_pPose = KRG::New<Pose>( m_pGraphVariation->GetSkeleton() );
        m_pPose->CalculateGlobalTransforms();

        m_pTaskSystem = KRG::New<TaskSystem>( m_pGraphVariation->GetSkeleton() );

        #if KRG_DEVELOPMENT_TOOLS
        m_graphContext.Initialize( GetEntityID().m_ID, m_pTaskSystem, m_pPose, m_pRootMotionActionRecorder );
        #else
        m_graphContext.Initialize( GetEntityID().m_ID, m_pTaskSystem, m_pPose, nullptr );
        #endif

        m_pGraphInstance = KRG::New<GraphInstance>( m_pGraphVariation.GetPtr() );
    }

    void AnimationGraphComponent::Shutdown()
    {
        if ( m_pGraphInstance->IsInitialized() )
        {
            m_pGraphInstance->Shutdown( m_graphContext );
        }
        m_graphContext.Shutdown();

        #if KRG_DEVELOPMENT_TOOLS
        KRG::Delete( m_pRootMotionActionRecorder );
        #endif

        KRG::Delete( m_pTaskSystem );
        KRG::Delete( m_pGraphInstance );
        KRG::Delete( m_pPose );

        EntityComponent::Shutdown();
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

    void AnimationGraphComponent::EvaluateGraph( Seconds deltaTime, Transform const& characterWorldTransform, Physics::Scene* pPhysicsScene )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        m_graphContext.Update( deltaTime, characterWorldTransform, pPhysicsScene );

        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder->StartCharacterUpdate( characterWorldTransform );
        #endif

        // Initialize graph on the first update
        if ( !m_pGraphInstance->IsInitialized() )
        {
            m_pGraphInstance->Initialize( m_graphContext );
        }

        // Reset last frame's tasks
        m_pTaskSystem->Reset();

        // Update the graph and record the root motion
        GraphPoseNodeResult const result = m_pGraphInstance->UpdateGraph( m_graphContext );
        m_rootMotionDelta = result.m_rootMotionDelta;
    }

    void AnimationGraphComponent::ExecutePrePhysicsTasks( Transform const& characterWorldTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();

        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder->EndCharacterUpdate( characterWorldTransform );
        #endif

        m_pTaskSystem->UpdatePrePhysics( m_graphContext.m_deltaTime, characterWorldTransform, characterWorldTransform.GetInverse() );
    }

    void AnimationGraphComponent::ExecutePostPhysicsTasks()
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        m_pTaskSystem->UpdatePostPhysics( *m_pPose );
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void AnimationGraphComponent::DrawDebug( Drawing::DrawContext& drawingContext )
    {
        m_pTaskSystem->DrawDebug( drawingContext );
        m_graphContext.GetRootMotionActionRecorder()->DrawDebug( drawingContext );
    }
    #endif
}