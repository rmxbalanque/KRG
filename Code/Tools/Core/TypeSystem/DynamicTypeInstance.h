#pragma once

#include "DynamicPropertyInstance.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_CORE_API DynamicTypeInstance final : private DynamicPropertyInstance
    {
    public:

        DynamicTypeInstance() = default;
        DynamicTypeInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo );

        virtual bool IsValid() const final
        {
            return m_pTypeInfo != nullptr && m_pTypeInfo->m_ID.IsValid();
        }

        inline TypeSystem::TypeID const& GetTypeID() const
        {
            return m_pTypeInfo->m_ID;
        }

        using DynamicPropertyInstance::GetTypeInfo;
        using DynamicPropertyInstance::GetProperty;

        using DynamicPropertyInstance::HasProperties;
        using DynamicPropertyInstance::GetProperties;
    };
}