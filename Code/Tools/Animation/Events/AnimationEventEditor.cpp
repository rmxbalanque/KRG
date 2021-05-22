#include "AnimationEventEditor.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Tools/Core/Thirdparty/pfd/portable-file-dialogs.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "System/DevTools/System/ImguiFont.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct EventManipulator
    {
        static void SetEventTime( Event* pEvent, Seconds startTime, Seconds duration )
        {
            KRG_ASSERT( pEvent != nullptr && duration >= 0 );
            pEvent->m_startTime = startTime;
            pEvent->m_duration = duration;
        }
    };
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    EventItem::EventItem( Event* pEvent, float frameRate )
        : m_pEvent ( pEvent)
        , m_animFrameRate( frameRate )
    {
        KRG_ASSERT( m_pEvent != nullptr && m_animFrameRate > 0 );
    }

    EventItem::~EventItem()
    {
        KRG::Delete( m_pEvent );
    }

    InlineString<100> EventItem::GetLabel() const
    {
        return m_pEvent->GetDisplayText();
    }

    FloatRange EventItem::GetTimeRange() const
    {
        KRG_ASSERT( m_animFrameRate != 0 && m_pEvent != nullptr );

        // Convert to frame-time
        float const startTime = m_pEvent->GetStartTime().ToFloat() * m_animFrameRate;
        float const duration = m_pEvent->GetDuration().ToFloat() * m_animFrameRate;
        float const endTime = startTime + duration;

        return FloatRange( startTime, endTime );
    }

    void EventItem::SetTimeRange( FloatRange const& inRange )
    {
        KRG_ASSERT( m_animFrameRate != 0 && m_pEvent != nullptr );

        // Convert to seconds
        float const startTime = inRange.m_start / m_animFrameRate;
        float const duration = inRange.GetLength() / m_animFrameRate;
        EventManipulator::SetEventTime( m_pEvent, startTime, duration );
    }

    //-------------------------------------------------------------------------

    const char* EventTrack::GetLabel() const
    {
        return reinterpret_cast<Event const*>( m_trackInfo.m_pEventTypeInfo->m_pTypeHelper->GetDefaultTypeInstancePtr() )->GetEventName();
    }

    void EventTrack::CreateItem( float itemStartTime )
    {
        KRG_ASSERT( m_animFrameRate > 0 );

        Seconds const startTime = itemStartTime / m_animFrameRate;

        //-------------------------------------------------------------------------

        auto pNewEvent = (Event*) m_trackInfo.m_pEventTypeInfo->m_pTypeHelper->CreateType();

        if ( m_trackInfo.m_eventType == Event::EventType::Immediate )
        {
            EventManipulator::SetEventTime( pNewEvent, startTime, 0.0f );
        }
        else
        {
            EventManipulator::SetEventTime( pNewEvent, startTime, 1.0f / m_animFrameRate );
        }

        m_items.emplace_back( KRG::New<EventItem>( pNewEvent, m_animFrameRate ) );
    };

    void EventTrack::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        if ( m_trackInfo.m_isSyncTrack )
        {
            ImGui::Text( KRG_ICON_HANDSHAKE_O " %s", GetLabel() );
        }
        else
        {
            ImGui::Text( GetLabel() );
        }
    }

    void EventTrack::DrawContextMenu( TVector<Editor::TimelineTrack*>& tracks, float playheadPosition )
    {
        if ( m_trackInfo.m_isSyncTrack )
        {
            if ( ImGui::MenuItem( "Clear Sync Track" ) )
            {
                m_trackInfo.m_isSyncTrack = false;
            }
        }
        else // Allow setting of sync track
        {
            if ( ImGui::MenuItem( "Set As Sync Track" ) )
            {
                // Clear sync track from any other track
                for ( auto pTrack : tracks )
                {
                    static_cast<EventTrack*>( pTrack )->m_trackInfo.m_isSyncTrack = false;
                }

                m_trackInfo.m_isSyncTrack = true;
            }
        }
    }

    //-------------------------------------------------------------------------

    EventEditor::EventEditor( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataDirectory, AnimationClip const* pAnimation )
        : Editor::TimelineEditor( IntRange( 0, pAnimation->GetNumFrames() ) )
        , m_typeRegistry( typeRegistry )
        , m_sourceDataDirectory( sourceDataDirectory )
        , m_eventTypes( typeRegistry.GetAllDerivedTypes( Event::GetStaticTypeID() ) )
        , m_animationID( pAnimation->GetResourceID() )
        , m_pAnimation( pAnimation )
    {
        KRG_ASSERT( m_pAnimation != nullptr );

        // Read resource desc to get event data path if set
        AnimationClipResourceDescriptor animResourceDesc;
        auto const resourceDescriptorPath = m_pAnimation->GetResourceID().GetDataPath().ToFileSystemPath( sourceDataDirectory );
        TypeSystem::Serialization::TypeReader typeReader( typeRegistry );
        if ( typeReader.ReadFromFile( resourceDescriptorPath ) )
        {
            typeReader >> animResourceDesc;
        }

        // Load the event data if set
        if ( animResourceDesc.m_animationEventData.IsValid() )
        {
            m_eventDataFilePath = animResourceDesc.m_animationEventData.ToFileSystemPath( sourceDataDirectory );
            LoadFromFile();
        }
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
                for ( auto pTrack : m_tracks )
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
                        pCreatedTrack->m_trackInfo.m_eventTypeID = pTypeInfo->m_ID;
                        pCreatedTrack->m_trackInfo.m_pEventTypeInfo = pTypeInfo;
                        pCreatedTrack->m_trackInfo.m_eventType = type;
                        m_tracks.emplace_back( pCreatedTrack );
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

    bool EventEditor::LoadFromFile()
    {
        auto LoadFunction = [this] ()
        {
            if ( !m_eventDataFilePath.Exists() )
            {
                return true;
            }

            // Read json data
            //-------------------------------------------------------------------------

            JsonFileReader trackReader;
            if ( !trackReader.ReadFromFile( m_eventDataFilePath ) )
            {
                KRG_LOG_ERROR( "AnimationTools", "Failed to read event track file: %s", m_eventDataFilePath.c_str() );
                return false;
            }

            if ( !trackReader.GetDocument().IsArray() )
            {
                KRG_LOG_ERROR( "AnimationTools", "Malformed event track file: %s", m_eventDataFilePath.c_str() );
                return false;
            }

            // Read event track data
            //-------------------------------------------------------------------------

            int32 numSyncTracks = 0;

            for ( auto& trackObjectValue : trackReader.GetDocument().GetArray() )
            {
                if ( !trackObjectValue.IsObject() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Malformed event track file" );
                    return false;
                }

                // Read track info
                //-------------------------------------------------------------------------

                auto trackInfoIter = trackObjectValue.FindMember( "TrackInfo" );
                if ( trackInfoIter == trackObjectValue.MemberEnd() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Malformed event track file" );
                    return false;
                }

                auto const& trackInfoValue = trackInfoIter->value;
                if ( !trackInfoValue.IsObject() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Malformed event track file" );
                    return false;
                }

                auto pCreatedTrack = KRG::New<EventTrack>();
                m_tracks.emplace_back( pCreatedTrack );

                if ( !TypeSystem::Serialization::ReadNativeType( m_typeRegistry, trackInfoValue, &pCreatedTrack->m_trackInfo ) )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Failed reading track info descriptor" );
                    return false;
                }

                if ( !pCreatedTrack->m_trackInfo.m_eventTypeID.IsValid() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Track info missing event type ID" );
                    return false;
                }

                pCreatedTrack->m_trackInfo.m_pEventTypeInfo = m_typeRegistry.GetTypeInfo( pCreatedTrack->m_trackInfo.m_eventTypeID );
                if ( pCreatedTrack->m_trackInfo.m_pEventTypeInfo == nullptr )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Unknown event type encountered: %s", pCreatedTrack->m_trackInfo.m_eventTypeID.c_str() );
                    return false;
                }

                //-------------------------------------------------------------------------

                pCreatedTrack->m_animFrameRate = m_pAnimation->GetFPS();

                if ( pCreatedTrack->m_trackInfo.m_isSyncTrack )
                {
                    numSyncTracks++;
                }

                // Read events
                //-------------------------------------------------------------------------

                auto eventsArrayIter = trackObjectValue.FindMember( "Events" );
                if ( eventsArrayIter == trackObjectValue.MemberEnd() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Malformed event track file" );
                    return false;
                }

                auto const& eventsArray = eventsArrayIter->value;
                if ( !eventsArray.IsArray() )
                {
                    KRG_LOG_ERROR( "AnimationTools", "Malformed event track file" );
                    return false;
                }

                for ( auto& eventObjectValue : eventsArrayIter->value.GetArray() )
                {
                    TypeSystem::TypeDescriptor eventDesc;
                    if ( !TypeSystem::Serialization::ReadTypeDescriptor( m_typeRegistry, eventObjectValue, eventDesc ) )
                    {
                        KRG_LOG_ERROR( "AnimationTools", "Failed reading event descriptor" );
                        return false;
                    }

                    // Event ownership is transfered to the item
                    auto pEvent = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Event>( m_typeRegistry, eventDesc );
                    auto pCreatedEvent = pCreatedTrack->m_items.emplace_back( KRG::New<EventItem>( pEvent, 30.0f ) );
                }
            }

            // Set sync track
            //-------------------------------------------------------------------------

            if ( numSyncTracks > 1 )
            {
                KRG_LOG_WARNING( "AnimationTools", "Multiple sync tracks encountered, using the first one found" );
            }

            //-------------------------------------------------------------------------

            return true;
        };

        //-------------------------------------------------------------------------

        if ( !LoadFunction() )
        {
            // Clean up any created tracks
            for ( auto pTrack : m_tracks )
            {
                KRG::Delete( pTrack );
            }
            m_tracks.clear();

            return false;
        }

        return true;
    }

    void EventEditor::Save()
    {
        if ( !m_eventDataFilePath.IsValid() )
        {
            FileSystem::Path const resourceDescPath = m_pAnimation->GetResourceID().GetDataPath().ToFileSystemPath( m_sourceDataDirectory );
            
            FileSystem::Path defaultEventDatafilePath = resourceDescPath;
            defaultEventDatafilePath.ReplaceExtension( "evnt" );

            pfd::save_file saveDialog( "Save Event Data", defaultEventDatafilePath.c_str(), { "Event Files (.evnt)", "*.evnt", } );
            m_eventDataFilePath = saveDialog.result().c_str();
           
            if ( m_eventDataFilePath.IsValid() )
            {
                TypeSystem::Serialization::TypeReader typeReader( m_typeRegistry );
                if ( typeReader.ReadFromFile( resourceDescPath ) )
                {
                    // Read and update resource desc
                    AnimationClipResourceDescriptor resourceDesc;
                    typeReader >> resourceDesc;
                    resourceDesc.m_animationEventData = DataPath::FromFileSystemPath( m_sourceDataDirectory, m_eventDataFilePath );

                    // Save resource desc
                    TypeSystem::Serialization::TypeWriter typeWriter( m_typeRegistry );
                    typeWriter << &resourceDesc;
                    typeWriter.WriteToFile( resourceDescPath );
                }
            }
            else
            {
                return;
            }
        }

        //-------------------------------------------------------------------------

        JsonFileWriter fileWriter;
        auto pWriter = fileWriter.GetWriter();

        pWriter->StartArray();

        //-------------------------------------------------------------------------

        for ( auto pTrack : m_tracks )
        {
            pWriter->StartObject();

            pWriter->Key( "TrackInfo" );

            auto pEventTrack = static_cast<EventTrack*>( pTrack );
            TypeSystem::Serialization::WriteNativeType( m_typeRegistry, *pWriter, EventTrackInfo::GetStaticTypeID(), &pEventTrack->m_trackInfo );

            //-------------------------------------------------------------------------

            pWriter->Key( "Events" );
            pWriter->StartArray();

            for ( auto pItem : pEventTrack->m_items )
            {
                auto pEvent = static_cast<EventItem*>( pItem );
                TypeSystem::Serialization::WriteNativeType( m_typeRegistry, *pWriter, pEvent->m_pEvent->GetTypeInfo()->m_ID, pEvent->m_pEvent );
            }

            pWriter->EndArray();
            pWriter->EndObject();
        }

        //-------------------------------------------------------------------------

        pWriter->EndArray();
        fileWriter.WriteToFile( m_eventDataFilePath );
        m_isDirty = false;
    }
}