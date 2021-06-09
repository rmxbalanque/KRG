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

namespace KRG
{
    class KRG_TOOLS_CORE_API TimelineItem : public IRegisteredType
    {
        friend class Editor;
        KRG_REGISTER_TYPE( TimelineItem );

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

        // Serialization
        //-------------------------------------------------------------------------

        // Optional function called when loading to initialize the event time range if not stored in the event itself
        virtual void InitializeTimeRange( FloatRange const& inRange ) {}

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& dataObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    private:

        TimelineItem( TimelineItem const& ) = delete;
        TimelineItem& operator=( TimelineItem& ) = delete;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API TimelineTrack : public IRegisteredType
    {
        friend class Editor;
        KRG_REGISTER_TYPE( TimelineTrack );

    public:

        TimelineTrack() = default;
        virtual ~TimelineTrack();

        // Info
        //-------------------------------------------------------------------------

        virtual const char* GetLabel() const { return "Track"; }
        virtual void DrawHeader( ImRect const& headerRect );

        // UI
        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu( TVector<TimelineTrack*>& tracks, float playheadPosition ) {}

        // Items
        //-------------------------------------------------------------------------

        inline bool Contains( TimelineItem const* pItem ) const { return eastl::find( m_items.begin(), m_items.end(), pItem ) != m_items.end(); }

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

        // Serialization
        //-------------------------------------------------------------------------

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    private:

        TimelineTrack( TimelineTrack const& ) = delete;
        TimelineTrack& operator=( TimelineTrack& ) = delete;

    public:

        TVector<TimelineItem*>      m_items;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API TimelineData
    {
        friend class Editor;

    public:

        // Frees all tracks
        void Reset();

        inline int32 GetNumTracks() const { return (int32) m_tracks.size(); }

        inline TVector<TimelineTrack*>::iterator begin() { return m_tracks.begin(); }
        inline TVector<TimelineTrack*>::iterator end() { return m_tracks.end(); }

        inline TimelineTrack*& operator[]( size_t i ) { return m_tracks[i]; }
        inline TimelineTrack* const& operator[]( size_t i ) const { return m_tracks[i]; }

        bool Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );
        void Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

    public:

        TVector<TimelineTrack*>     m_tracks;
    };
}