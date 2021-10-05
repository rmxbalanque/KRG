#pragma once
#include "TypeID.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API ResourceInfo
    {
        inline bool IsValid() const { return m_typeID.IsValid() && m_resourceTypeID.IsValid(); }

    public:

        TypeID                  m_typeID;
        ResourceTypeID          m_resourceTypeID;

        #if KRG_DEVELOPMENT_TOOLS
        String                  m_friendlyName;
        #endif
    };
}