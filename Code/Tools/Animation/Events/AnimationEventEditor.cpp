#include "AnimationEventEditor.h"
#include "Tools/Animation/ResourceCompilers/ResourceCompiler_AnimationClip.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    EventEditor::EventEditor( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataDirectory, AnimationClip const* pAnimation )
        : TimelineEditor( IntRange( 0, pAnimation->GetNumFrames() ) )
        , m_typeRegistry( typeRegistry )
        , m_sourceDataDirectory( sourceDataDirectory )
        , m_eventTypes( typeRegistry.GetAllDerivedTypes( Event::GetStaticTypeID() ) )
        , m_animationID( pAnimation->GetResourceID() )
        , m_pAnimation( pAnimation )
    {
        KRG_ASSERT( m_pAnimation != nullptr );
        LoadEventData();
    }

    //-------------------------------------------------------------------------

    void EventEditor::Update( UpdateContext const& context, AnimationPlayerComponent* pPreviewAnimationComponent )
    {
        KRG_ASSERT( pPreviewAnimationComponent != nullptr );

        // Handle play state changes
        //-------------------------------------------------------------------------

        if ( IsPlaying() && pPreviewAnimationComponent->IsPosed() )
        {
            pPreviewAnimationComponent->SetPlayMode( IsLoopingEnabled() ? AnimationPlayerComponent::PlayMode::Loop : AnimationPlayerComponent::PlayMode::PlayOnce );
            pPreviewAnimationComponent->SetAnimTime( GetPlayheadPositionAsPercentage() );
        }
        else if ( IsPaused() && !pPreviewAnimationComponent->IsPosed() )
        {
            pPreviewAnimationComponent->SetPlayMode( AnimationPlayerComponent::PlayMode::Posed );
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

    bool EventEditor::DrawAddTracksMenu()
    {
        bool result = false;
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
                        auto pCreatedTrack = KRG::New<EventTrack>();
                        pCreatedTrack->m_animFrameRate = m_pAnimation->GetFPS();
                        pCreatedTrack->m_eventTypeID = pTypeInfo->m_ID;
                        pCreatedTrack->m_pEventTypeInfo = pTypeInfo;
                        pCreatedTrack->m_eventType = type;
                        m_trackContainer.m_tracks.emplace_back( pCreatedTrack );
                        return true;
                    }

                    return false;
                };

                //-------------------------------------------------------------------------

                if ( pDefaultEventInstance->GetEventType() == Event::EventType::Both )
                {
                    result |= CreateTrackOption( Event::EventType::Immediate, "(Immediate)" );
                    result |= CreateTrackOption( Event::EventType::Duration, "(Duration)" );
                }
                else
                {
                    result |= CreateTrackOption( pDefaultEventInstance->GetEventType() );
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( numAvailableTracks == 0 )
        {
            ImGui::Text( "No Available Tracks" );
        }

        return result;
    }

    //-------------------------------------------------------------------------

    void EventEditor::LoadEventData()
    {
        ClearSelection();
        m_trackContainer.Reset();

        //-------------------------------------------------------------------------

        auto const resourceDescriptorPath = m_pAnimation->GetResourceID().GetDataPath().ToFileSystemPath( m_sourceDataDirectory );
        JsonReader typeReader;
        if ( !typeReader.ReadFromFile( resourceDescriptorPath ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Failed to read resource descriptor file: %s", resourceDescriptorPath.c_str() );
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
            KRG_LOG_ERROR( "AnimationTools", "Malformed event track file: %s", resourceDescriptorPath.c_str() );
            return;
        }

        //-------------------------------------------------------------------------

        int32 numSyncTracks = 0;
        for ( auto pTrack : m_trackContainer.m_tracks )
        {
            auto pEventTrack = reinterpret_cast<EventTrack*>( pTrack );
            pEventTrack->m_animFrameRate = m_pAnimation->GetFPS();

            for ( auto pItem : pEventTrack->m_items )
            {
                auto pEventItem = Cast<EventItem>( pItem );
                pEventItem->m_animFrameRate = m_pAnimation->GetFPS();
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
        auto const resourceDescriptorPath = m_pAnimation->GetResourceID().GetDataPath().ToFileSystemPath( m_sourceDataDirectory );
        JsonReader jsonReader;
        if ( !jsonReader.ReadFromFile( resourceDescriptorPath ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Failed to read resource descriptor file: %s", resourceDescriptorPath.c_str() );
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
        eventDataWriter.WriteToFile( resourceDescriptorPath );
        m_isDirty = false;
        return true;
    }
}