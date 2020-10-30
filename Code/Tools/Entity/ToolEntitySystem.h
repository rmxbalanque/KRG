#pragma once

#include "_Module/API.h"
#include "System/TypeSystem/TypeInfo.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API ToolEntitySystem
    {
        friend class ToolEntity;
        friend class ToolEntityCollectionConverter;

    public:

        ToolEntitySystem( TypeSystem::ToolTypeInstance const& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        ToolEntitySystem( TypeSystem::ToolTypeInstance&& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        ToolEntitySystem( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo )
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

        TypeSystem::ToolTypeInstance        m_typeInstance;
        UUID                                m_parentEntityID;
    };
}