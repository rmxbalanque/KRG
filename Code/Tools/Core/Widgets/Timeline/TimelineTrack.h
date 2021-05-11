#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

struct ImRect;

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    class KRG_TOOLS_CORE_API Item
    {
        friend class Editor;

    public:

        Item() = default;
        virtual ~Item() = default;

        // Basic Info
        //-------------------------------------------------------------------------

        virtual InlineString<100> GetLabel() const { return "Item"; }
        virtual Color GetColor() const { return Color( 0xFF801080 ); }

        // Time manipulation
        //-------------------------------------------------------------------------
        // This is done to abstract the timeline's time format from the item's format since they may differ

        virtual TRange<float> GetTimeRange() const = 0;
        virtual void SetTimeRange( TRange<float> const& inRange ) = 0;

        inline bool IsImmediateItem() const { return GetTimeRange().m_min == GetTimeRange().m_max; }
        inline bool IsDurationItem() const { return GetTimeRange().m_min != GetTimeRange().m_max; }
        inline float GetLength() const { return GetTimeRange().GetLength(); }

        // Context menu
        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu() {}

    private:

        Item( Item const& ) = delete;
        Item& operator=( Item& ) = delete;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API Track
    {
        friend class Editor;

    public:

        Track() = default;
        virtual ~Track();

        //-------------------------------------------------------------------------

        virtual const char* GetLabel() const { return "Track"; }
        virtual void DrawHeader( ImRect const& headerRect );

        //-------------------------------------------------------------------------

        inline bool Contains( Item const* pItem ) const { return eastl::find( m_items.begin(), m_items.end(), pItem ) != m_items.end(); }

        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu( float playheadPosition ) {}

        //-------------------------------------------------------------------------

        // Needs to be implemented by the derived track
        virtual void CreateItem( float itemStartTime ) {};

        // Try to delete the item from the track if it exists, return true if the item was found and remove, false otherwise
        inline bool DeleteItem( Item* pItem )
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

        Track( Track const& ) = delete;
        Track& operator=( Track& ) = delete;

    public:

        TVector<Item*>      m_items;
    };
}