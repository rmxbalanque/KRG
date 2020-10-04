#pragma once

#include "_Module/API.h"
#include "System/Entity/Entity.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/LoadingStatus.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    namespace Serialization{ class EntityArchive; }

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityCollection
    {

    public:

        EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, Serialization::EntityArchive const& entityArchive );
        virtual ~EntityCollection();

        inline UUID GetID() const { return m_ID; }
        TVector<Entity*> const& GetEntities() const { return m_entities; }
        inline bool ContainsEntity( UUID entityID ) const { return FindEntity( entityID ) != nullptr; }

    private:

        EntityCollection( UUID ID );

        inline Entity* FindEntity( UUID entityID ) const
        {
            auto iter = m_entityLookupMap.find( entityID );
            return ( iter != m_entityLookupMap.end() ) ? iter->second : nullptr;
        }

    private:

        UUID                                m_ID;
        TVector<Entity*>                    m_entities;
        THashMap<UUID, Entity*>             m_entityLookupMap;
    };
}