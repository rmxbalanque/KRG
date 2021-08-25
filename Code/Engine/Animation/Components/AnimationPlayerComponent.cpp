#include "AnimationPlayerComponent.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationPlayerComponent::SetAnimation( ResourceID animationResourceID )
    {
        KRG_ASSERT( IsUnloaded() );
        KRG_ASSERT( animationResourceID.IsValid() );
        m_pAnimation = animationResourceID;
    }

    void AnimationPlayerComponent::SetPlayMode( PlayMode mode )
    {
        m_playMode = mode;
        m_previousAnimTime = Percentage( -1 );
    }

    void AnimationPlayerComponent::SetAnimTime( Percentage inTime )
    {
        m_animTime = inTime.GetClamped( m_playMode == PlayMode::Loop );
    }

    void AnimationPlayerComponent::SetAnimTime( Seconds inTime )
    {
        KRG_ASSERT( m_pAnimation != nullptr && m_pAnimation.IsLoaded() );
        Percentage const percentage( inTime / m_pAnimation->GetDuration() );
        SetAnimTime( percentage );
    }

    //-------------------------------------------------------------------------

    void AnimationPlayerComponent::Initialize()
    {
        AnimationComponent::Initialize();
        KRG_ASSERT( m_pAnimation != nullptr && m_pAnimation.IsLoaded() );
        m_pPose = KRG::New<Pose>( m_pAnimation->GetSkeleton() );
    }

    void AnimationPlayerComponent::Shutdown()
    {
        KRG::Delete( m_pPose );
        m_previousAnimTime = -1.0f;
        AnimationComponent::Shutdown();
    }

    //-------------------------------------------------------------------------

    Skeleton const* AnimationPlayerComponent::GetSkeleton() const
    {
        KRG_ASSERT( m_pAnimation != nullptr );
        return m_pAnimation->GetSkeleton();
    }

    void AnimationPlayerComponent::PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        KRG_ASSERT( m_pAnimation != nullptr && m_pPose != nullptr );

        //-------------------------------------------------------------------------

        bool bShouldUpdate = false;

        switch ( m_playMode )
        {
            case PlayMode::Loop:
            {
                m_previousAnimTime = m_animTime;
                m_animTime += Percentage( deltaTime / m_pAnimation->GetDuration() );
                m_animTime = m_animTime.GetClamped( true );
                bShouldUpdate = true;
            }
            break;

            case PlayMode::PlayOnce:
            {
                if ( m_previousAnimTime < 1.0f )
                {
                    m_previousAnimTime = m_animTime;
                    m_animTime += Percentage( deltaTime / m_pAnimation->GetDuration() );
                    m_animTime = m_animTime.GetClamped( false );
                    bShouldUpdate = true;
                }
            }
            break;

            case PlayMode::Posed:
            {
                if ( m_previousAnimTime != m_animTime )
                {
                    m_previousAnimTime = m_animTime;
                    bShouldUpdate = true;
                }
            }
            break;
        }

        //-------------------------------------------------------------------------

        if ( bShouldUpdate )
        {
            m_pAnimation->GetPose( m_animTime, m_pPose );
            m_pPose->CalculateGlobalTransforms();

            // Check if we've looped
            if( m_animTime < m_previousAnimTime )
            {
                Transform const preLoopDelta = m_pAnimation->GetRootMotionDelta( m_previousAnimTime, 1.0f );
                Transform const postLoopDelta = m_pAnimation->GetRootMotionDelta( 0.0f, m_animTime );
                m_rootMotionDelta = postLoopDelta * preLoopDelta;
            }
            else
            {
                m_rootMotionDelta = m_pAnimation->GetRootMotionDelta( m_previousAnimTime, m_animTime );
            }
        }
        else // Clear the root motion delta
        {
            m_rootMotionDelta = Transform::Identity;
        }
    }
}