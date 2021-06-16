#include "AnimationEventEditor.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Tools/Core/Thirdparty/pfd/portable-file-dialogs.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "System/DevTools/System/ImguiFont.h"
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
                for ( auto pTrack : m_data )
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
                        m_data.m_tracks.emplace_back( pCreatedTrack );
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
        JsonFileReader jsonReader;
        if ( !jsonReader.ReadFromFile( m_eventDataFilePath ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Failed to read event track file: %s", m_eventDataFilePath.c_str() );
            return false;
        }

        //-------------------------------------------------------------------------

        if ( !m_data.Load( m_typeRegistry, jsonReader.GetDocument() ) )
        {
            KRG_LOG_ERROR( "AnimationTools", "Malformed event track file: %s", m_eventDataFilePath.c_str() );
            return false;
        }

        int32 numSyncTracks = 0;
        for ( auto pTrack : m_data.m_tracks )
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

        //-------------------------------------------------------------------------

        return true;
    }

    void EventEditor::RequestSave()
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
        m_data.Save( m_typeRegistry, *fileWriter.GetWriter() );
        fileWriter.WriteToFile( m_eventDataFilePath );
        m_isDirty = false;
    }
}