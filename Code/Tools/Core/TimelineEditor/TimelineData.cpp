#include "TimelineData.h"
#include "imgui.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    Track::~Track()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void Track::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( GetLabel() );
    }

    bool Track::IsDirty() const
    {
        if ( m_isDirty )
        {
            return true;
        }

        for ( auto const pItem : m_items )
        {
            if ( pItem->IsDirty() )
            {
                return true;
            }
        }

        return false;
    }

    void Track::ClearDirtyFlags()
    {
        m_isDirty = false;

        for ( auto pItem : m_items )
        {
            pItem->ClearDirtyFlag();
        }
    }

    //-------------------------------------------------------------------------

    void TrackContainer::Reset()
    {
        for ( auto pTrack : m_tracks )
        {
            KRG::Delete( pTrack );
        }
        m_tracks.clear();
        m_isDirty = false;
    }

    bool TrackContainer::Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& dataObjectValue )
    {
        auto FreeTrackData = [this] ()
        {
            for ( auto pTrack : m_tracks )
            {
                KRG::Delete( pTrack );
            }
            m_tracks.clear();
        };

        FreeTrackData();

        //-------------------------------------------------------------------------

        if ( !dataObjectValue.IsArray() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        bool failed = false;

        for ( auto const& trackObjectValue : dataObjectValue.GetArray() )
        {
            auto trackDataIter = trackObjectValue.FindMember( "TrackData" );
            if ( trackDataIter == trackObjectValue.MemberEnd() )
            {
                failed = true;
                break;
            }

            // Create track
            Track* pTrack = TypeSystem::Serialization::CreateAndReadNativeType<Track>( typeRegistry, trackDataIter->value );
            m_tracks.emplace_back( pTrack );

            // Custom serialization
            pTrack->SerializeCustom( typeRegistry, trackObjectValue );

            //-------------------------------------------------------------------------

            auto itemArrayIter = trackObjectValue.FindMember( "Items" );
            if ( itemArrayIter == trackObjectValue.MemberEnd() || !itemArrayIter->value.IsArray() )
            {
                failed = true;
                break;
            }

            //-------------------------------------------------------------------------

            // Deserialize items
            for ( auto const& itemObjectValue : itemArrayIter->value.GetArray() )
            {
                auto itemDataIter = itemObjectValue.FindMember( "ItemData" );
                auto startTimeDataIter = itemObjectValue.FindMember( "StartTime" );
                auto endTimeDataIter = itemObjectValue.FindMember( "EndTime" );
                if ( itemDataIter == itemObjectValue.MemberEnd() || startTimeDataIter == itemObjectValue.MemberEnd() || endTimeDataIter == itemObjectValue.MemberEnd() )
                {
                    failed = true;
                    break;
                }

                // Create item
                TrackItem* pItem = TypeSystem::Serialization::CreateAndReadNativeType<TrackItem>( typeRegistry, itemDataIter->value );
                pTrack->m_items.emplace_back( pItem );

                // Custom serialization
                pItem->SerializeCustom( typeRegistry, itemObjectValue );

                // Set time range
                FloatRange itemTimeRange;
                itemTimeRange.m_start = (float) startTimeDataIter->value.GetDouble();
                itemTimeRange.m_end = (float) endTimeDataIter->value.GetDouble();
                pItem->InitializeTimeRange( itemTimeRange );
            }

            if ( failed )
            {
                break;
            }
        }

        //-------------------------------------------------------------------------

        if ( failed )
        {
            FreeTrackData();
            m_isDirty = false;
            return false;
        }

        ClearDirtyFlags();
        return true;
    }

    void TrackContainer::Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer )
    {
        writer.StartArray();

        //-------------------------------------------------------------------------

        for ( auto pTrack : m_tracks )
        {
            writer.StartObject();

            // Track
            //-------------------------------------------------------------------------

            writer.Key( "TrackData" );
            TypeSystem::Serialization::WriteNativeType( typeRegistry, writer, pTrack );

            pTrack->SerializeCustom( typeRegistry, writer );

            // Items
            //-------------------------------------------------------------------------

            writer.Key( "Items" );
            writer.StartArray();
            {
                for ( auto pItem : pTrack->m_items )
                {
                    writer.StartObject();
                    {
                        writer.Key( "ItemData" );
                        TypeSystem::Serialization::WriteNativeType( typeRegistry, writer, pItem );

                        auto const& itemTimeRange = pItem->GetTimeRange();
                        writer.Key( "StartTime" );
                        writer.Double( itemTimeRange.m_start );

                        writer.Key( "EndTime" );
                        writer.Double( itemTimeRange.m_end );

                        pItem->SerializeCustom( typeRegistry, writer );
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();

            //-------------------------------------------------------------------------

            writer.EndObject();
        }

        //-------------------------------------------------------------------------

        writer.EndArray();
        ClearDirtyFlags();
    }

    bool TrackContainer::IsDirty() const
    {
        if ( m_isDirty )
        {
            return true;
        }

        for ( auto const pTrack : m_tracks )
        {
            if ( pTrack->IsDirty() )
            {
                return true;
            }
        }

        return false;
    }

    void TrackContainer::ClearDirtyFlags()
    {
        m_isDirty = false;

        for ( auto pTrack : m_tracks )
        {
            pTrack->ClearDirtyFlags();
        }
    }

    bool TrackContainer::Contains( Track const* pTrack ) const
    {
        return eastl::find( m_tracks.begin(), m_tracks.end(), pTrack ) != m_tracks.end();
    }

    bool TrackContainer::Contains( TrackItem const* pItem ) const
    {
        for ( auto pTrack : m_tracks )
        {
            if ( pTrack->Contains( pItem ) )
            {
                return true;
            }
        }

        return false;
    }

    void TrackContainer::DeleteTrack( Track* pTrack )
    {
        KRG_ASSERT( Contains( pTrack ) );
        m_tracks.erase_first( pTrack );
        KRG::Delete( pTrack );
        m_isDirty = true;
    }
}