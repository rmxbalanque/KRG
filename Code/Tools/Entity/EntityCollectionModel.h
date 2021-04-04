#pragma once

#include "_Module/API.h"
#include "EntityInstanceModel.h"
#include "System/Entity/Collections/EntityCollection.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Math/Matrix.h"

//-------------------------------------------------------------------------
// A collection of entities
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityCollectionDescriptor;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API EntityCollectionModel
    {
        friend struct EntityCollectionConverter;

    public:

        static bool FromDescriptor( EntityCollectionDescriptor const& inCollectionDesc, EntityCollectionModel& outModel );

    public:

        EntityCollectionModel( TypeSystem::TypeRegistry const& typeRegistry );
        ~EntityCollectionModel();

        //-------------------------------------------------------------------------

        EntityCollectionDescriptor GetDescriptor() const;

        // Entity Access
        //-------------------------------------------------------------------------

        inline void AddEntity( EntityInstanceModel* pEntity )
        {
            KRG_ASSERT( pEntity != nullptr && pEntity->IsValid() && !pEntity->IsOwnedByEntity() );
            m_entities.push_back( pEntity );
        }

        void Clear() { m_entities.clear(); }

        inline int32 GetNumEntities() const { return (int32) m_entities.size(); }

        inline TVector<EntityInstanceModel*>& GetEntities() { return m_entities; }

        inline TVector<EntityInstanceModel*> const& GetEntities() const { return m_entities; }

        inline EntityInstanceModel const* FindEntity( UUID const& entityID ) const { return TryFindEntity( entityID ); }
        
        inline EntityInstanceModel* FindEntity( UUID const& entityID ) { return const_cast<EntityInstanceModel*>( TryFindEntity( entityID ) ); }

        inline int32 FindEntityIndex( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            size_t const numEntities = m_entities.size();
            for ( auto i = 0; i < numEntities; i++ )
            {
                if ( m_entities[i]->GetID() == entityID )
                {
                    return (int32) i;
                }
            }

            return InvalidIndex;
        }

        // Component Access
        //-------------------------------------------------------------------------

        TVector<EntityComponentModel const*> GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes = true ) const;

    private:

        inline EntityInstanceModel const* TryFindEntity( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            for ( auto& pEntity : m_entities )
            {
                if ( pEntity->GetID() == entityID )
                {
                    return pEntity;
                }
            }

            return nullptr;
        }

    protected:

        TypeSystem::TypeRegistry const&                     m_typeRegistry;
        TVector<EntityInstanceModel*>                       m_entities;
    };
}