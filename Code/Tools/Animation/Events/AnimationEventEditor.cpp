#include "AnimationEventEditor.h"
#include "AnimationEventData.h"
#include "Engine/Animation/Components/Component_AnimationClipPlayer.h"
#include "Engine/Animation/AnimationEvent.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    EventEditor::EventEditor( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& descriptorPath, uint32 numFrames, float FPS )
        : TimelineEditor( IntRange( 0, numFrames ) )
        , m_typeRegistry( typeRegistry )
        , m_descriptorPath( descriptorPath )
        , m_FPS( FPS )
        , m_eventTypes( typeRegistry.GetAllDerivedTypes( Event::GetStaticTypeID() ) )
    {
        KRG_ASSERT( m_FPS > 0.0f );
        LoadEventData();
    }

    //-------------------------------------------------------------------------

    void EventEditor::UpdateAndDraw( UpdateContext const& context, AnimationClipPlayerComponent* pPreviewAnimationComponent )
    {
        KRG_ASSERT( pPreviewAnimationComponent != nullptr );

        // Handle play state changes
        //-------------------------------------------------------------------------

        if ( IsPlaying() && pPreviewAnimationComponent->IsPosed() )
        {
            pPreviewAnimationComponent->SetPlayMode( IsLoopingEnabled() ? AnimationClipPlayerComponent::PlayMode::Loop : AnimationClipPlayerComponent::PlayMode::PlayOnce );
            pPreviewAnimationComponent->SetAnimTime( GetPlayheadPositionAsPercentage() );
        }
        else if ( IsPaused() && !pPreviewAnimationComponent->IsPosed() )
        {
            pPreviewAnimationComponent->SetPlayMode( AnimationClipPlayerComponent::PlayMode::Posed );
        }

        // Draw track editor and manage playhead data
        //-------------------------------------------------------------------------

        // If we are playing, then update the playhead position
        if ( IsPlaying() )
        {
            SetPlayheadPositionAsPercentage( pPreviewAnimationComponent->GetAnimTime() );
        }

        Draw();

        // Is we are paused, then update the animation component pose
        if ( IsPaused() )
        {
            pPreviewAnimationComponent->SetAnimTime( GetPlayheadPositionAsPercentage() );
        }
    }

    void EventEditor::DrawAddTracksMenu()
    {
        int32 numAvailableTracks = 0;
        for ( auto pTypeInfo : m_eventTypes )
        {
            Event const* const pDefaultEventInstance = reinterpret_cast<Event const*>( pTypeInfo->m_pTypeHelper->GetDefaultTypeInstancePtr() );

            bool isAllowedTrackType = true;
            if ( !pDefaultEventInstance->AllowMultipleTracks() )
            {
                // Check if a track of this type already exists
                for ( auto pTrack : m_trackContainer )
                {
                    auto pAnimTrack = static_cast<EventTrack*>( pTrack );
                    if ( pAnimTrack->GetEventTypeInfo() == pDefaultEventInstance->GetTypeInfo() )
                    {
                        isAllowedTrackType = false;
                        break;
                    }
                }
            }

            //-------------------------------------------------------------------------

            if ( isAllowedTrackType )
            {
                numAvailableTracks++;

                auto CreateTrackOption = [this, pDefaultEventInstance, pTypeInfo] ( Event::EventType type, char const* pNameSuffix = nullptr )
                {
                    InlineString<255> menuItemName = pDefaultEventInstance->GetEventName();
                    if ( pNameSuffix != nullptr )
                    {
                        menuItemName += pNameSuffix;
                    }

                    if ( ImGui::MenuItem( menuItemName.c_str() ) )
                    {
                        auto pCreatedTrack = m_trackContainer.CreateTrack<EventTrack>();
                        pCreatedTrack->m_animFrameRate = m_FPS;
                        pCreatedTrack->m_eventTypeID = pTypeInfo->m_ID;
                        pCreatedTrack->m_pEventTypeInfo = pTypeInfo;
                        pCreatedTrack->m_eventType = type;
                    }
                };

                //-------------------------------------------------------------------------

                if ( pDefaultEventInstance->GetEventType() == Event::EventType::Both )
                {
                    CreateTrackOption( Event::EventType::Immediate, "(Immediate)" );
                    CreateTrackOption( Event::EventType::Duration, "(Duration)" );
                }
                else
                {
                    CreateTrackOption( pDefaultEventInstance->GetEventType() );
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( numAvailableTracks == 0 )
        {
            ImGui::Text( "No Available Tracks" );
        }
    }

    //-------------------------------------------------------------------------

    void EventEditor::LoadEventData()
    {
        ClearSelection();
        m_trackContainer.Reset();

        //-------------------------------------------------------------------------

        JsonReader typeReader;
        if ( !typeReader.ReadFromFile( m_descriptorPath ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Failed to read resource descriptor file: %s", m_descriptorPath.c_str() );
            return;
        }

        // Check if there is event data
        auto const& document = typeReader.GetDocument();
        auto trackDataIter = document.FindMember( EventTrack::s_eventTrackContainerKey );
        if ( trackDataIter == document.MemberEnd() )
        {
            return;
        }

        auto const& eventDataValueObject = trackDataIter->value;
        if ( !eventDataValueObject.IsArray() )
        {
            KRG_LOG_ERROR( "AnimationTools", "Malformed event track data" );
            return;
        }

        if ( !m_trackContainer.Load( m_typeRegistry, eventDataValueObject ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Malformed event track file: %s", m_descriptorPath.c_str() );
            return;
        }

        //-------------------------------------------------------------------------

        int32 numSyncTracks = 0;
        for ( auto pTrack : m_trackContainer.m_tracks )
        {
            auto pEventTrack = reinterpret_cast<EventTrack*>( pTrack );
            pEventTrack->m_animFrameRate = m_FPS;

            for ( auto pItem : pEventTrack->m_items )
            {
                auto pEventItem = Cast<EventItem>( pItem );
                pEventItem->m_animFrameRate = m_FPS;
            }

            if ( pEventTrack->m_isSyncTrack )
            {
                numSyncTracks++;
                
                // If we have more than one sync track, clear the rest
                if ( numSyncTracks > 1 )
                {
                    pEventTrack->m_isSyncTrack = false;
                }
            }
        }
    }

    bool EventEditor::RequestSave()
    {
        JsonReader jsonReader;
        if ( !jsonReader.ReadFromFile( m_descriptorPath ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Failed to read resource descriptor file: %s", m_descriptorPath.c_str() );
            return false;
        }

        auto const& document = jsonReader.GetDocument();

        //-------------------------------------------------------------------------

        // Serialize the event data
        JsonWriter eventDataWriter;

        auto pWriter = eventDataWriter.GetWriter();
        pWriter->StartObject();

        // Write descriptor data
        for ( auto itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr )
        {
            if ( itr->name == EventTrack::s_eventTrackContainerKey )
            {
                continue;
            }

            pWriter->Key( itr->name.GetString() );
            itr->value.Accept( *pWriter );
        }

        // Write event data
        pWriter->Key( EventTrack::s_eventTrackContainerKey );
        m_trackContainer.Save( m_typeRegistry, *eventDataWriter.GetWriter() );

        pWriter->EndObject();

        // Save descriptor
        eventDataWriter.WriteToFile( m_descriptorPath );
        return true;
    }
}