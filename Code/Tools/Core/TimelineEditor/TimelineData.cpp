#include "TimelineData.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TimelineTrack::~TimelineTrack()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void TimelineTrack::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( GetLabel() );
    }

    //-------------------------------------------------------------------------

    void TimelineData::Reset()
    {
        for ( auto pTrack : m_tracks )
        {
            KRG::Delete( pTrack );
        }
        m_tracks.clear();
    }

    bool TimelineData::Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& dataObjectValue )
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
            TimelineTrack* pTrack = TypeSystem::Serialization::CreateAndReadNativeType<TimelineTrack>( typeRegistry, trackDataIter->value );
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
                TimelineItem* pItem = TypeSystem::Serialization::CreateAndReadNativeType<TimelineItem>( typeRegistry, itemDataIter->value );
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
            return false;
        }

        return true;
    }

    void TimelineData::Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
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
    }

}