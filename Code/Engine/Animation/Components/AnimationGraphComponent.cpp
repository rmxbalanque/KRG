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
    }

    void AnimationGraphComponent::Shutdown()
    {
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
        KRG_ASSERT( m_pGraph != nullptr && m_pDataSet != nullptr );

        //-------------------------------------------------------------------------

        m_pPose->Reset();
        m_pPose->CalculateGlobalTransforms();
    }
}