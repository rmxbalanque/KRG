#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/NumericRange.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

struct ImRect;

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    class KRG_TOOLS_CORE_API TrackItem : public IRegisteredType
    {
        KRG_REGISTER_TYPE( TrackItem );

    public:

        TrackItem() = default;
        virtual ~TrackItem() = default;

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

        // Serialization
        //-------------------------------------------------------------------------

        // Optional function called when loading to initialize the event time range if not stored in the event itself
        virtual void InitializeTimeRange( FloatRange const& inRange ) {}

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& dataObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    private:

        TrackItem( TrackItem const& ) = delete;
        TrackItem& operator=( TrackItem& ) = delete;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API Track : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Track );

    public:

        Track() = default;
        virtual ~Track();

        // Info
        //-------------------------------------------------------------------------

        virtual const char* GetLabel() const { return "Track"; }
        virtual void DrawHeader( ImRect const& headerRect );

        // UI
        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu( TVector<Track*>& tracks, float playheadPosition ) {}

        // Items
        //-------------------------------------------------------------------------

        inline bool Contains( TrackItem const* pItem ) const { return eastl::find( m_items.begin(), m_items.end(), pItem ) != m_items.end(); }

        // Needs to be implemented by the derived track
        virtual void CreateItem( float itemStartTime ) {};

        // Try to delete the item from the track if it exists, return true if the item was found and remove, false otherwise
        inline bool DeleteItem( TrackItem* pItem )
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

        // Serialization
        //-------------------------------------------------------------------------

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    private:

        Track( Track const& ) = delete;
        Track& operator=( Track& ) = delete;

    public:

        TVector<TrackItem*>      m_items;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API TrackContainer
    {
    public:

        // Frees all tracks
        void Reset();

        inline int32 GetNumTracks() const { return (int32) m_tracks.size(); }

        inline TVector<Track*>::iterator begin() { return m_tracks.begin(); }
        inline TVector<Track*>::iterator end() { return m_tracks.end(); }

        inline Track*& operator[]( size_t i ) { return m_tracks[i]; }
        inline Track* const& operator[]( size_t i ) const { return m_tracks[i]; }

        bool Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );
        void Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

    public:

        TVector<Track*>     m_tracks;
    };
}