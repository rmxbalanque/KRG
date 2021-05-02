#pragma once

#include "Applications/Editor/Common/Timeline/TimelineEditor.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "Engine/Animation/AnimationEvent.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG { class UpdateContext; }
namespace KRG::Animation { class AnimationPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationEvent final : public Timeline::Item
    {
    public:

        AnimationEvent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, float startTime, float duration = 1.0f );

        virtual const char* GetLabel() const override;
        inline TypeSystem::TypeInstanceModel* GetTypeInstance() { return &m_typeInstance; }
        inline TypeSystem::TypeInstanceModel const* GetTypeInstance() const { return &m_typeInstance; }

    private:

        TypeSystem::TypeInstanceModel               m_typeInstance;
    };

    //-------------------------------------------------------------------------

    class AnimationEventTrack final : public Timeline::Track
    {
    public:

        AnimationEventTrack( TypeSystem::TypeRegistry const& typeRegistry, Event const* pDefaultEventInstance );

        virtual const char* GetLabel() const override;
        virtual void CreateItem( float itemStartTime ) override;
        inline TypeSystem::TypeInfo const* GetEventTypeInfo() const { return m_pDefaultEventInstance->GetTypeInfo(); }

    private:

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        Event const*                                m_pDefaultEventInstance = nullptr;
    };

    //-------------------------------------------------------------------------

    class AnimationEventEditor final : public Timeline::Editor
    {
    public:

        AnimationEventEditor( TypeSystem::TypeRegistry const& typeRegistry, AnimationClip const* pAnimation );

        inline ResourceID const& GetAnimationID() const { return m_animationID; }
        TInlineVector<TypeSystem::TypeInstanceModel*, 5> GetSelectedEventInstances() const;

        void Update( UpdateContext const& context, AnimationPlayerComponent* pPreviewAnimationComponent );
        virtual void DrawAddTracksMenu() override;

    private:

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        TVector<TypeSystem::TypeInfo const*>        m_eventTypes;
        ResourceID                                  m_animationID; // This is tracked independently since the animation ptr can be invalidated externally
        AnimationClip const*                        m_pAnimation = nullptr;
    };
}