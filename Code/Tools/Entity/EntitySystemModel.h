#pragma once

#include "_Module/API.h"
#include "EntityPropertyInstanceModel.h"
#include "System/TypeSystem/TypeInfo.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API EntitySystemModel
    {
        friend class EntityInstanceModel;
        friend class EntityCollectionModelConverter;

    public:

        EntitySystemModel( TypeSystem::TypeInstanceModel const& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        EntitySystemModel( TypeSystem::TypeInstanceModel&& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        EntitySystemModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo )
            : m_typeInstance( typeRegistry, pTypeInfo )
        {
            KRG_ASSERT( m_typeInstance.IsValid() );
            KRG_ASSERT( m_typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        inline bool IsValid() const 
        {
            return m_typeInstance.IsValid();
        }

        inline TypeSystem::TypeInfo const* GetTypeInfo() const 
        {
            KRG_ASSERT( IsValid() );
            return m_typeInstance.GetTypeInfo();
        }

        inline TypeSystem::TypeID GetTypeID() const 
        { 
            KRG_ASSERT( IsValid() );
            return m_typeInstance.GetTypeID();
        }

        inline bool IsOwnedByEntity() const
        {
            return m_parentEntityID.IsValid();
        }

    private:

        TypeSystem::TypeInstanceModel           m_typeInstance;
        UUID                                    m_parentEntityID;
    };
}