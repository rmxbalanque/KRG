#pragma once

#include "../_Module/API.h"
#include "EntityMap.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityMapLoader : public Resource::ResourceLoader
    {
    public:

        EntityMapLoader();
        void SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry );

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryArchive& archive ) const final;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry;
    };
}