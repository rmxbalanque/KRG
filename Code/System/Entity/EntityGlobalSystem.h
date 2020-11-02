#pragma once

#include "_Module/API.h"
#include "System/Core/Update/UpdateStage.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------
// Global entity system
//-------------------------------------------------------------------------
// This is a global system that exists once per world and tracks/updates all components of certain types in the world!

namespace KRG
{
    class SystemRegistry;
    class UpdateContext;
    class Entity;
    class EntityComponent;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API IGlobalEntitySystem
    {
        friend class EntityWorld;

        static UpdatePriorityList const PriorityList;

    public:

        virtual U32 GetEntitySystemID() const = 0;

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() = 0;

        // Called when the system is registered with the world - using explicit "EntitySystem" name to allow for a standalone initialize function
        virtual void InitializeEntitySystem( SystemRegistry const& systemRegistry ) {};

        // Called when the system is removed from the world - using explicit "EntitySystem" name to allow for a standalone shutdown function
        virtual void ShutdownEntitySystem() {};

        // System Update - using explicit "EntitySystem" name to allow for a standalone update functions
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) {};

        // Called whenever a new component is activated (i.e. added to the world)
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) = 0;

        // Called immediately before an component is deactivated
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_ENTITY_GLOBAL_SYSTEM( Type )\
    constexpr static U32 const EntitySystemID = Hash::FNV1a::GetHash32( #Type );\
    virtual U32 GetEntitySystemID() const override final { return Type::EntitySystemID; }\
    static UpdatePriorityList const PriorityList;\
    virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { return Type::PriorityList; };

//-------------------------------------------------------------------------
// Entity registry
//-------------------------------------------------------------------------
// This is a helper data structure that allows for fast insertion and removal of a record based on entity ID
// This is needed since some global systems can track hundreds of thousands of entities and the unregistration of entities becomes extremely costly
// TODO: right now we never compact the allocated memory within the registry so it will always remain at the largest size needed.

namespace KRG
{
    // A base class for a record, needed to track whether a record is set or not
    struct EntityRegistryRecord
    {
        template<typename T> friend class EntityRegistry;

    public:

        KRG_FORCE_INLINE bool IsSet() const { return m_isSet; }

    private:

        bool m_isSet = false;
    };

    //-------------------------------------------------------------------------

    // The registry class, tries to act like a flat array as much as possible but insertion/deletion has to be done via entity ID
    template<typename RecordType>
    class EntityRegistry
    {
        static_assert( std::is_base_of<EntityRegistryRecord, RecordType>::value, "Invalid specialization for EntityRegistry, only classes derived from RegisteredEntityRecord are allowed." );

    public:

        KRG_FORCE_INLINE bool IsEmpty() const { return m_recordIndices.size() == 0; }
        KRG_FORCE_INLINE S32 GetNumRegisteredEntities() const { return (S32) m_recordIndices.size(); }

        // Flat array access
        //-------------------------------------------------------------------------

        // Fast access to records as if they were inline
        KRG_FORCE_INLINE TVector<RecordType> const& GetRecords() { return m_records; }
        KRG_FORCE_INLINE RecordType& operator[]( S32 idx ) { return m_records[idx]; }
        KRG_FORCE_INLINE S32 size() const { return (S32) m_records.size(); }

        KRG_FORCE_INLINE typename TVector<RecordType>::iterator begin() { return m_records.begin(); }
        KRG_FORCE_INLINE typename TVector<RecordType>::iterator end() { return m_records.end(); }

        KRG_FORCE_INLINE typename TVector<RecordType>::const_iterator begin() const { return m_records.begin(); }
        KRG_FORCE_INLINE typename TVector<RecordType>::const_iterator end() const { return m_records.end(); }

        // Insertion / Deletion / Search
        //-------------------------------------------------------------------------

        // Add a new record
        RecordType& AddRecord( UUID entityID )
        {
            KRG_ASSERT( entityID.IsValid() && m_recordIndices.find( entityID ) == m_recordIndices.end() );

            S32 const recordIdx = m_firstFreeIdx;
            m_firstFreeIdx++;

            // Update the free idx tracker
            S32 const numRecords = (S32) m_records.size();
            for ( m_firstFreeIdx; m_firstFreeIdx < numRecords; m_firstFreeIdx++ )
            {
                if ( !m_records[m_firstFreeIdx].m_isSet )
                {
                    break;
                }
            }

            // Add record
            m_recordIndices.insert( TPair<UUID, S32>( entityID, recordIdx ) );
            if ( recordIdx >= numRecords )
            {
                auto& newRecord = m_records.emplace_back();
                newRecord.m_isSet = true;
                return newRecord;
            }
            else
            {
                m_records[recordIdx].m_isSet = true;
                return m_records[recordIdx];
            }
        }

        // Remove a record - leave memory intact
        KRG_FORCE_INLINE void RemoveRecord( UUID entityID )
        {
            KRG_ASSERT( entityID.IsValid() );
            auto foundIdx = m_recordIndices.find( entityID );
            m_records[foundIdx->second].m_isSet = false;
            m_firstFreeIdx = Math::Min( m_firstFreeIdx, foundIdx->second );
            m_recordIndices.erase( foundIdx );
        }

        KRG_FORCE_INLINE RecordType* operator[]( UUID entityID )
        {
            RecordType* pFoundRecord = nullptr;

            auto foundIdx = m_recordIndices.find( entityID );
            if ( foundIdx != m_recordIndices.end() )
            {
                pFoundRecord = &m_records[foundIdx->second];
            }

            return pFoundRecord;
        }

        KRG_FORCE_INLINE bool HasRecordForEntity( UUID entityID ) const
        {
            return m_recordIndices.find( entityID ) != m_recordIndices.end();
        }

    private:

        TVector<RecordType>                 m_records;
        THashMap<UUID, S32>                 m_recordIndices;
        S32                                 m_firstFreeIdx = 0;
    };
}