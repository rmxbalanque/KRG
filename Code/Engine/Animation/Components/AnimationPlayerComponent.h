#pragma once

#include "Engine/Animation/_Module/API.h"
#include "AnimationComponent.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationPlayerComponent final : public AnimationComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationPlayerComponent );

    public:

        enum class PlayMode
        {
            KRG_REGISTER_ENUM

            PlayOnce,
            Loop,
            Posed
        };

    public:

        inline AnimationPlayerComponent() = default;
        inline AnimationPlayerComponent( StringID name ) : AnimationComponent( UUID::GenerateID(), name ) {}

        //-------------------------------------------------------------------------

        virtual Skeleton const* GetSkeleton() const override;
        virtual Pose const* GetPose() const override { return m_pPose; }
        virtual Transform const& GetRootMotionDelta() const override { return m_rootMotionDelta; }
        virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override;
        virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override {}

        //-------------------------------------------------------------------------

        // This function will change the animation resource currently played! Note: this can only be called for unloaded components
        void SetAnimation( ResourceID animationResourceID );

        inline Percentage GetAnimTime() const { return m_animTime; }
        inline ResourceID const& GetAnimationID() const { return m_pAnimation->GetResourceID(); }

        // Play Controls
        //-------------------------------------------------------------------------

        inline bool IsPosed() const { return m_playMode == PlayMode::Posed; }

        // Set's the play mode for this component. Can be called at any time
        void SetPlayMode( PlayMode mode );

        // This is the preferred method for setting the animation time for the component. Can be called at any time
        void SetAnimTime( Percentage inTime );

        // This is a helper that automatically converts seconds to percentage and sets the time! Note: this can only be called for initialized components
        void SetAnimTime( Seconds inTime );

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

    private:

        KRG_EXPOSE TResourcePtr<AnimationClip>          m_pAnimation = nullptr;
        KRG_EXPOSE PlayMode                             m_playMode = PlayMode::Loop;

        Pose*                                           m_pPose = nullptr;
        Transform                                       m_rootMotionDelta = Transform::Identity;
        Percentage                                      m_previousAnimTime = Percentage( 0.0f );
        Percentage                                      m_animTime = Percentage( 0.0f );
    };
}