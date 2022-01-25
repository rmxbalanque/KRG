#pragma once

#include "Engine/Animation/AnimationClip.h"
#include "Engine/Core/Entity/EntityComponent.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationClipPlayerComponent final : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationClipPlayerComponent );

    public:

        enum class PlayMode
        {
            KRG_REGISTER_ENUM

            PlayOnce,
            Loop,
            Posed
        };

    public:

        inline AnimationClipPlayerComponent() = default;
        inline AnimationClipPlayerComponent( StringID name ) : EntityComponent( name ) {}

        void Update( Seconds deltaTime, Transform const& characterTransform );

        //-------------------------------------------------------------------------

        Skeleton const* GetSkeleton() const;
        Pose const* GetPose() const { return m_pPose; }
        inline Transform const& GetRootMotionDelta() const { return m_rootMotionDelta; }
        inline bool RequiresManualUpdate() const { return m_requiresManualUpdate; }

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

        Transform                                       m_rootMotionDelta = Transform::Identity;
        Pose*                                           m_pPose = nullptr;
        Percentage                                      m_previousAnimTime = Percentage( 0.0f );
        Percentage                                      m_animTime = Percentage( 0.0f );
        KRG_EXPOSE bool                                 m_requiresManualUpdate = false;
    };
}