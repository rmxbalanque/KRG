#pragma once

#include "Tools/Editor/_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/NumericRange.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

struct ImRect;

//-------------------------------------------------------------------------

namespace KRG::Editor
{
    class KRG_TOOLS_EDITOR_API TimelineItem
    {
        friend class Editor;

    public:

        TimelineItem() = default;
        virtual ~TimelineItem() = default;

        // Basic Info
        //-------------------------------------------------------------------------

        virtual InlineString<100> GetLabel() const { return "Item"; }
        virtual Color GetColor() const { return Color( 0xFF801080 ); }

        // Time manipulation
        //-------------------------------------------------------------------------
        // This is done to abstract the timeline's time format from the item's format since they may differ

        virtual FloatRange GetTimeRange() const = 0;
        virtual void SetTimeRange( FloatRange const& inRange ) = 0;

        inline bool IsImmediateItem() const { return GetTimeRange().m_start == GetTimeRange().m_end; }
        inline bool IsDurationItem() const { return GetTimeRange().m_start != GetTimeRange().m_end; }
        inline float GetLength() const { return GetTimeRange().GetLength(); }

        // Context menu
        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu() {}

    private:

        TimelineItem( TimelineItem const& ) = delete;
        TimelineItem& operator=( TimelineItem& ) = delete;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_EDITOR_API TimelineTrack
    {
        friend class Editor;

    public:

        TimelineTrack() = default;
        virtual ~TimelineTrack();

        //-------------------------------------------------------------------------

        virtual const char* GetLabel() const { return "Track"; }
        virtual void DrawHeader( ImRect const& headerRect );

        //-------------------------------------------------------------------------

        inline bool Contains( TimelineItem const* pItem ) const { return eastl::find( m_items.begin(), m_items.end(), pItem ) != m_items.end(); }

        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu( TVector<TimelineTrack*>& tracks, float playheadPosition ) {}

        //-------------------------------------------------------------------------

        // Needs to be implemented by the derived track
        virtual void CreateItem( float itemStartTime ) {};

        // Try to delete the item from the track if it exists, return true if the item was found and remove, false otherwise
        inline bool DeleteItem( TimelineItem* pItem )
        {
            auto foundIter = eastl::find( m_items.begin(), m_items.end(), pItem );
            if ( foundIter != m_items.end() )
            {
                KRG::Delete( *foundIter );
                m_items.erase( foundIter );
                return true;
            }

            return false;
        }

    private:

        TimelineTrack( TimelineTrack const& ) = delete;
        TimelineTrack& operator=( TimelineTrack& ) = delete;

    public:

        TVector<TimelineItem*>      m_items;
    };
}