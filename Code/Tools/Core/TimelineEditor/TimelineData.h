#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/ThirdParty/KRG_RapidJson.h"
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

        // Dirty state
        //-------------------------------------------------------------------------

        inline bool IsDirty() const { return m_isDirty; }
        inline void MarkDirty() { m_isDirty = true; }
        inline void ClearDirtyFlag() { m_isDirty = false; }

        // Basic Info
        //-------------------------------------------------------------------------

        virtual InlineString GetLabel() const { return "Item"; }
        virtual Color GetColor() const { return Color( 0xFF801080 ); }

        // Time manipulation
        //-------------------------------------------------------------------------
        // This is done to abstract the timeline's time format from the item's format since they may differ

        virtual FloatRange GetTimeRange() const = 0;

        void SetTimeRange( FloatRange const& inRange )
        {
            SetTimeRangeInternal( inRange );
            m_isDirty = true;
        }

        inline void ClampToRange( FloatRange const& validRange )
        {
            KRG_ASSERT( validRange.IsSetAndValid() );

            FloatRange clampedRange = GetTimeRange();
            clampedRange.m_start = validRange.GetClampedValue( clampedRange.m_start );
            clampedRange.m_end = validRange.GetClampedValue( clampedRange.m_end );
            KRG_ASSERT( clampedRange.IsSetAndValid() );
            SetTimeRangeInternal( clampedRange );
        }

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

    protected:

        virtual void SetTimeRangeInternal( FloatRange const& inRange ) = 0;

    private:

        TrackItem( TrackItem const& ) = delete;
        TrackItem& operator=( TrackItem& ) = delete;

    private:

        bool                    m_isDirty = false;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API Track : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Track );
        friend class TrackContainer;

    public:

        Track() = default;
        virtual ~Track();

        // Info
        //-------------------------------------------------------------------------

        virtual const char* GetLabel() const { return "Track"; }
        virtual void DrawHeader( ImRect const& headerRect );

        bool IsDirty() const;
        void ClearDirtyFlags();
        inline void MarkDirty() { m_isDirty = true; }

        // UI
        //-------------------------------------------------------------------------

        virtual bool HasContextMenu() const { return false; }
        virtual void DrawContextMenu( TVector<Track*>& tracks, float playheadPosition ) {}

        // Items
        //-------------------------------------------------------------------------

        inline bool Contains( TrackItem const* pItem ) const { return eastl::find( m_items.begin(), m_items.end(), pItem ) != m_items.end(); }

        inline TVector<TrackItem*> const& GetItems() const { return m_items; }

        // Create a new item at the specified start time
        inline void CreateItem( float itemStartTime )
        {
            CreateItemInternal( itemStartTime );
            m_isDirty = true;
        };

        // Try to delete the item from the track if it exists, return true if the item was found and remove, false otherwise
        inline bool DeleteItem( TrackItem* pItem )
        {
            auto foundIter = eastl::find( m_items.begin(), m_items.end(), pItem );
            if ( foundIter != m_items.end() )
            {
                KRG::Delete( *foundIter );
                m_items.erase( foundIter );
                m_isDirty = true;
                return true;
            }

            return false;
        }

        // Serialization
        //-------------------------------------------------------------------------

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    private:

        // Needs to be implemented by the derived track
        virtual void CreateItemInternal( float itemStartTime ) = 0;

        Track( Track const& ) = delete;
        Track& operator=( Track& ) = delete;

    protected:

        TVector<TrackItem*>      m_items;
        bool                     m_isDirty = false;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API TrackContainer
    {
    public:

        // Frees all tracks
        void Reset();

        // Dirty state
        //-------------------------------------------------------------------------

        bool IsDirty() const;
        void ClearDirtyFlags();
        inline void MarkDirty() { m_isDirty = true; }

        // Tracks
        //-------------------------------------------------------------------------

        inline TVector<Track*>::iterator begin() { return m_tracks.begin(); }
        inline TVector<Track*>::iterator end() { return m_tracks.end(); }

        inline Track*& operator[]( size_t i ) { return m_tracks[i]; }
        inline Track* const& operator[]( size_t i ) const { return m_tracks[i]; }

        template<typename T> T* CreateTrack()
        {
            static_assert( std::is_base_of<Track, T>::value, "T has to derive from Timeline::Track" );
            auto pCreatedTrack = static_cast<T*>( m_tracks.emplace_back( KRG::New<T>() ) );
            m_isDirty = true;
            return pCreatedTrack;
        }

        void DeleteTrack( Track* pTrack );

        // Query
        //-------------------------------------------------------------------------

        inline int32 GetNumTracks() const { return (int32) m_tracks.size(); }
        bool Contains( Track const* pTrack ) const;
        bool Contains( TrackItem const* pItem ) const;

        // Serialization
        //-------------------------------------------------------------------------

        bool Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );
        void Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer );

    public:

        TVector<Track*>     m_tracks;
        bool                m_isDirty = false;
    };
}