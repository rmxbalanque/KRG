#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/TimelineEditor/TimelineData.h"
#include "Engine/Animation/AnimationEvent.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------
// Stores tools only information for event track data
//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EventItem final : public TimelineItem
    {
        friend class EventEditor;
        KRG_REGISTER_TYPE( EventItem );

    public:

        EventItem() = default;
        EventItem( Event* pEvent, float frameRate );
        ~EventItem();

        virtual InlineString<100> GetLabel() const override;
        virtual FloatRange GetTimeRange() const override;
        virtual void SetTimeRange( FloatRange const& inRange ) override;

        Event* GetEvent() const { return m_pEvent; }
        TypeSystem::TypeInfo const* GetEventTypeInfo() const { return m_pEvent->GetTypeInfo(); }

    private:

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue ) override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

    private:

        Event*                                      m_pEvent = nullptr;
        float                                       m_animFrameRate = 0.0f;
    };

    //-------------------------------------------------------------------------

    class EventTrack final : public TimelineTrack
    {
        friend class EventEditor;
        KRG_REGISTER_TYPE( EventTrack );

    public:

        EventTrack() = default;

        inline TypeSystem::TypeInfo const* GetEventTypeInfo() const { return m_pEventTypeInfo; }
        inline bool IsSyncTrack() const { return m_isSyncTrack; }

        virtual const char* GetLabel() const override;
        virtual void CreateItem( float itemStartTime ) override;
        virtual void DrawHeader( ImRect const& headerRect ) override;
        virtual bool HasContextMenu() const override { return true; }
        virtual void DrawContextMenu( TVector<TimelineTrack*>& tracks, float playheadPosition ) override;

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue ) override;

    private:

        KRG_EXPOSE Event::EventType                     m_eventType = Event::EventType::Duration;
        KRG_EXPOSE bool                                 m_isSyncTrack = false;
        KRG_EXPOSE TypeSystem::TypeID                   m_eventTypeID;
        TypeSystem::TypeInfo const*                 m_pEventTypeInfo = nullptr;
        float                                       m_animFrameRate = 0.0f;
    };
}