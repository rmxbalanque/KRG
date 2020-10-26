#include "SimpleAnimationComponent.h"
#include "System/Animation/AnimationPose.h"
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
            #if KRG_DEBUG_INSTRUMENTATION
            static DebugSettingBool g_showCompressionStats( "ShowCompressionStats", "Animation/Show Compression Stats", "", true );
            #endif
        }

        //-------------------------------------------------------------------------

        void SimpleAnimationComponent::SetAnimation( ResourceID animationResourceID )
        {
            KRG_ASSERT( IsUnloaded() );
            KRG_ASSERT( animationResourceID.IsValid() );
            m_pAnimation = animationResourceID;
        }

        //-------------------------------------------------------------------------

        void SimpleAnimationComponent::Initialize()
        {
            EntityComponent::Initialize();
            KRG_ASSERT( m_pAnimation != nullptr && m_pAnimation.IsLoaded() );
            m_pPose = KRG::New<Pose>( m_pAnimation->GetSkeleton() );
        }

        void SimpleAnimationComponent::Shutdown()
        {
            KRG::Delete( m_pPose );
            EntityComponent::Shutdown();
        }

        //-------------------------------------------------------------------------

        Skeleton const* SimpleAnimationComponent::GetSkeleton() const
        {
            KRG_ASSERT( m_pAnimation != nullptr );
            return m_pAnimation->GetSkeleton();
        }

        void SimpleAnimationComponent::Update( UpdateContext const& ctx )
        {
            KRG_ASSERT( m_pAnimation != nullptr && m_pPose != nullptr );

            m_animTime += Percentage( ctx.GetDeltaTime() / m_pAnimation->GetDuration() );
            m_animTime = m_animTime.GetClamped( true );
            m_pAnimation->GetPose( m_animTime, m_pPose );

            m_pPose->CalculateGlobalTransforms();
        }
    }
}