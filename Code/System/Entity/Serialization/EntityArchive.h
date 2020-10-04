#pragma once

#include "EntitySerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        class KRG_SYSTEM_ENTITY_API EntityArchive
        {
            KRG_SERIALIZE_MEMBERS( m_entities );

        public:

            inline EntityArchive& operator<<( SerializedEntity const& entityDesc )
            {
                m_entities.push_back( entityDesc );
                return *this;
            }

            // Entity Access
            //-------------------------------------------------------------------------

            inline SerializedEntity const* FindEntity( UUID const& entityID ) const
            {
                KRG_ASSERT( entityID.IsValid() );

                for ( auto& entity : m_entities )
                {
                    if ( entity.m_ID == entityID )
                    {
                        return &entity;
                    }
                }

                return nullptr;
            }

            inline S32 FindEntityIndex( UUID const& entityID ) const
            {
                KRG_ASSERT( entityID.IsValid() );

                size_t const numEntities = m_entities.size();
                for ( auto i = 0; i < numEntities; i++ )
                {
                    if ( m_entities[i].m_ID == entityID )
                    {
                        return (S32) i;
                    }
                }

                return InvalidIndex;
            }

            // Serialization
            //-------------------------------------------------------------------------

            inline TVector<SerializedEntity> const& GetEntityDescriptors() const { return m_entities; }

        protected:

            TVector<SerializedEntity>                                   m_entities;
        };
    }
}