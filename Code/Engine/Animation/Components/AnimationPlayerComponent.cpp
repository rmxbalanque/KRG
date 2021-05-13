#include "AnimationPlayerComponent.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Settings/DebugSettings.h"

//HACK

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Settings
        {
            #if KRG_DEVELOPMENT_TOOLS
            static DebugSettingBool g_showCompressionStats( "ShowCompressionStats", "Animation/Show Compression Stats", "", true );
            #endif
        }

        //-------------------------------------------------------------------------

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

        void AnimationPlayerComponent::Update( UpdateContext const& ctx )
        {
            KRG_ASSERT( m_pAnimation != nullptr && m_pPose != nullptr );

            //-------------------------------------------------------------------------

            bool bSamplePose = false;

            switch ( m_playMode )
            {
                case PlayMode::Loop:
                {
                    m_previousAnimTime = m_animTime;
                    m_animTime += Percentage( ctx.GetDeltaTime() / m_pAnimation->GetDuration() );
                    m_animTime = m_animTime.GetClamped( true );
                    bSamplePose = true;
                }
                break;

                case PlayMode::PlayOnce:
                {
                    if ( m_previousAnimTime < 1.0f )
                    {
                        m_previousAnimTime = m_animTime;
                        m_animTime += Percentage( ctx.GetDeltaTime() / m_pAnimation->GetDuration() );
                        m_animTime = m_animTime.GetClamped( false );
                        bSamplePose = true;
                    }
                }
                break;

                case PlayMode::Posed:
                {
                    if ( m_previousAnimTime != m_animTime )
                    {
                        m_previousAnimTime = m_animTime;
                        bSamplePose = true;
                    }
                }
                break;
            }

            //-------------------------------------------------------------------------

            if ( bSamplePose )
            {
                m_pAnimation->GetPose( m_animTime, m_pPose );
                m_pPose->CalculateGlobalTransforms();
            }
        }
    }
}