#pragma once

#include "_Module/API.h"
#include "System/TypeSystem/TypeInfo.h"
#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization { class ToolEntityCollectionReader; class ToolEntityCollectionWriter; }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API ToolEntitySystem
    {
        friend class Serialization::ToolEntityCollectionReader;
        friend class Serialization::ToolEntityCollectionWriter;

    public:

        ToolEntitySystem( DynamicTypeInstance const& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
        }

        ToolEntitySystem( DynamicTypeInstance&& typeInstance )
            : m_typeInstance( typeInstance )
        {
            KRG_ASSERT( typeInstance.IsValid() );
            KRG_ASSERT( typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) );
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

    private:

        DynamicTypeInstance m_typeInstance;
    };
}