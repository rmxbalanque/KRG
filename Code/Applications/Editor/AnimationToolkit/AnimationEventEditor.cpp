#include "AnimationEventEditor.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationEvent::AnimationEvent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, float startTime, float duration )
        : Item()
        , m_typeInstance( typeRegistry, pTypeInfo )
    {
        m_timeRange.m_min = startTime;
        m_timeRange.m_max = startTime + duration;
    }

    const char* AnimationEvent::GetLabel() const
    {
        return m_typeInstance.GetTypeInfo()->GetTypeName();
    }
}

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationEventTrack::AnimationEventTrack( TypeSystem::TypeRegistry const& typeRegistry, Event const* pDefaultEventInstance )
        : Track()
        , m_typeRegistry( typeRegistry )
        , m_pDefaultEventInstance( pDefaultEventInstance )
    {
        KRG_ASSERT( m_pDefaultEventInstance != nullptr );
    }

    const char* AnimationEventTrack::GetLabel() const
    {
        return m_pDefaultEventInstance->GetEventTypeName();
    }

    void AnimationEventTrack::CreateItem( float itemStartTime )
    {
        m_items.emplace_back( KRG::New<AnimationEvent>( m_typeRegistry, m_pDefaultEventInstance->GetTypeInfo(), itemStartTime ) );
    };
}

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationEventEditor::AnimationEventEditor( TypeSystem::TypeRegistry const& typeRegistry, AnimationClip const* pAnimation )
        : Timeline::Editor( TRange<int32>( 0, pAnimation->GetNumFrames() ) )
        , m_typeRegistry( typeRegistry )
        , m_eventTypes( typeRegistry.GetAllDerivedTypes( Event::GetStaticTypeID() ) )
        , m_animationID( pAnimation->GetResourceID() )
        , m_pAnimation( pAnimation )
    {}

    TInlineVector<TypeSystem::TypeInstanceModel*, 5> AnimationEventEditor::GetSelectedEventInstances() const
    {
        TInlineVector<TypeSystem::TypeInstanceModel*, 5> selectedInstances;

        for ( auto pSelectedItem : m_selectedItems )
        {
            auto pSelectedEvent = static_cast<AnimationEvent*>( pSelectedItem );
            selectedInstances.emplace_back( pSelectedEvent->GetTypeInstance() );
        }

        return selectedInstances;
    }

    //-------------------------------------------------------------------------

    void AnimationEventEditor::Update( UpdateContext const& context, AnimationPlayerComponent* pPreviewAnimationComponent )
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

    void AnimationEventEditor::DrawAddTracksMenu()
    {
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
                    auto pAnimTrack = static_cast<AnimationEventTrack*>( pTrack );
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
                if ( ImGui::MenuItem( pDefaultEventInstance->GetEventTypeName() ) )
                {
                    m_tracks.emplace_back( KRG::New<AnimationEventTrack>( m_typeRegistry, pDefaultEventInstance ) );
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( numAvailableTracks == 0 )
        {
            ImGui::Text( "No Available Tracks" );
        }
    }
}