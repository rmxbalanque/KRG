#pragma once
#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    //-------------------------------------------------------------------------
    // Entity Collection Template
    //-------------------------------------------------------------------------
    // This is a read-only resource that contains a collection of serialized entity descriptors
    // We used this to instantiate a collection of entities
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityCollectionTemplate
    {
        KRG_SERIALIZE_MEMBERS( m_entityDescriptors, m_entityLookupMap, m_entitySpatialAttachmentInfo );

        friend class EntityCollection;

    protected:

        struct SpatialAttachmentInfo
        {
            KRG_SERIALIZE_MEMBERS( m_entityIdx, m_parentEntityIdx );

            S32                                                         m_entityIdx = InvalidIndex;
            S32                                                         m_parentEntityIdx = InvalidIndex;
        };

    public:

        inline bool IsValid() const 
        {
            return m_entityDescriptors.size() > 0 && m_entityDescriptors.size() == m_entityLookupMap.size();
        }

        // Template Creation
        //-------------------------------------------------------------------------

        void Reserve( S32 numEntities );

        inline EntityCollectionTemplate& operator<<( EntityDescriptor const& entityDesc )
        {
            KRG_ASSERT( entityDesc.IsValid() );
            m_entityLookupMap.insert( TPair<UUID, S32>( entityDesc.m_ID, (S32) m_entityDescriptors.size() ) );
            m_entityDescriptors.push_back( entityDesc );
            return *this;
        }

        void GenerateSpatialAttachmentInfo();

        void Clear() { m_entityDescriptors.clear(); m_entityLookupMap.clear(); m_entitySpatialAttachmentInfo.clear(); }

        // Entity Access
        //-------------------------------------------------------------------------

        inline TVector<EntityDescriptor> const& GetEntityDescriptors() const
        { 
            return m_entityDescriptors; 
        }

        inline EntityDescriptor const* FindEntityDescriptor( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityID );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return &m_entityDescriptors[foundEntityIter->second];
            }
            else
            {
                return nullptr;
            }
        }

        inline S32 FindEntityIndex( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityID );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return foundEntityIter->second;
            }
            else
            {
                return InvalidIndex;
            }
        }

    protected:

        TVector<EntityDescriptor>                                   m_entityDescriptors;
        THashMap<UUID, S32>                                         m_entityLookupMap;
        TVector<SpatialAttachmentInfo>                              m_entitySpatialAttachmentInfo;
    };
}