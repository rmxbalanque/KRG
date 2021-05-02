#pragma once

#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

struct ImRect;

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    class Item
    {
        friend class Editor;

    public:

        Item() = default;
        virtual ~Item() = default;

        inline bool IsImmediateItem() const { return m_timeRange.m_min == m_timeRange.m_max; }
        inline bool IsDurationItem() const { return m_timeRange.m_min != m_timeRange.m_max; }
        inline float GetLength() const { return m_timeRange.GetLength(); }

        virtual char const* GetLabel() const { return "Item"; }
        virtual Color GetColor() const { return Color( 0xFF801080 ); }

        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu() const {}

    private:

        Item( Item const& ) = delete;
        Item& operator=( Item& ) = delete;

    public:

        TRange<float>       m_timeRange;
    };

    //-------------------------------------------------------------------------

    class Track
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
        virtual void DrawContextMenu( float playheadPosition ) const {}

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