#pragma once

#include "_Module/API.h"
#include "ToolEntity.h"
#include "System/Entity/Collections/EntityCollection.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Math/Matrix.h"

//-------------------------------------------------------------------------
// A collection of entities
//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization { class ToolEntityCollectionReader; }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API ToolEntityCollection
    {
        friend Serialization::ToolEntityCollectionReader;

    public:

        ToolEntityCollection( TypeSystem::TypeRegistry const& typeRegistry );

        inline ToolEntityCollection& operator<<( ToolEntity const& entity )
        {
            m_entities.push_back( entity );
            return *this;
        }

        void Reset() { m_entities.clear(); }

        // Entity Access
        //-------------------------------------------------------------------------

        inline S32 GetNumEntities() const { return (S32) m_entities.size(); }

        inline TVector<ToolEntity>& GetEntities() { return m_entities; }

        inline TVector<ToolEntity> const& GetEntities() const { return m_entities; }

        inline ToolEntity const* FindEntity( UUID const& entityID ) const { return TryFindEntity( entityID ); }
        
        inline ToolEntity* FindEntity( UUID const& entityID ) { return const_cast<ToolEntity*>( TryFindEntity( entityID ) ); }

        inline S32 FindEntityIndex( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            size_t const numEntities = m_entities.size();
            for ( auto i = 0; i < numEntities; i++ )
            {
                if ( m_entities[i].GetID() == entityID )
                {
                    return (S32) i;
                }
            }

            return InvalidIndex;
        }

        // Component Access
        //-------------------------------------------------------------------------

        TVector<ToolEntityComponent const*> GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes = true ) const;

    private:

        inline ToolEntity const* TryFindEntity( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            for ( auto& entity : m_entities )
            {
                if ( entity.GetID() == entityID )
                {
                    return &entity;
                }
            }

            return nullptr;
        }

    protected:

        TypeSystem::TypeRegistry const&                     m_typeRegistry;
        TVector<ToolEntity>                                 m_entities;
    };
}