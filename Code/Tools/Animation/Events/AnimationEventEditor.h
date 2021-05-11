#pragma once

#include "Tools/Animation/_Module/API.h"
#include "AnimationEventTrackInfo.h"
#include "Tools/Core/Widgets/Timeline/TimelineEditor.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "Engine/Animation/AnimationEvent.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG { class UpdateContext; }
namespace KRG::Animation { class AnimationPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class EventItem final : public Timeline::Item
    {
        friend class EventEditor;

    public:

        EventItem() = default;
        EventItem( Event* pEvent, float frameRate );
        ~EventItem();

        virtual InlineString<100> GetLabel() const override;
        virtual TRange<float> GetTimeRange() const;
        virtual void SetTimeRange( TRange<float> const& inRange );

        TypeSystem::TypeInfo const* GetEventTypeInfo() const { return m_pEvent->GetTypeInfo(); }

    private:

        Event*                                      m_pEvent = nullptr;
        float                                       m_animFrameRate = 0.0f;
    };
}
//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class EventTrack final : public Timeline::Track
    {
        friend class EventEditor;

    public:

        EventTrack() = default;

        inline TypeSystem::TypeInfo const* GetEventTypeInfo() const { return m_trackInfo.m_pEventTypeInfo; }

        virtual const char* GetLabel() const override;
        virtual void CreateItem( float itemStartTime ) override;
        virtual void DrawHeader( ImRect const& headerRect ) override;
        virtual bool HasContextMenu() const override { return true; }
        virtual void DrawContextMenu( float playheadPosition ) override;


    private:

        EventTrackInfo                              m_trackInfo;
    };
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class KRG_TOOLS_ANIMATION_API EventEditor final : public Timeline::Editor
    {
    public:

        EventEditor( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataDirectory, AnimationClip const* pAnimation );

        inline ResourceID const& GetAnimationID() const { return m_animationID; }

        void Update( UpdateContext const& context, AnimationPlayerComponent* pPreviewAnimationComponent );
        void SaveToFile();

    private:

        virtual void DrawAddTracksMenu() override;
        bool LoadFromFile();

    private:

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        FileSystem::Path const                      m_sourceDataDirectory;
        TVector<TypeSystem::TypeInfo const*>        m_eventTypes;
        ResourceID                                  m_animationID; // This is tracked independently since the animation ptr can be invalidated externally
        AnimationClip const*                        m_pAnimation = nullptr;
        FileSystem::Path                            m_eventDataFilePath;
    };
}