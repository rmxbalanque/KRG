#pragma once

#include "../_Module/API.h"
#include "System/Entity/Map/EntityMapDescriptor.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_SYSTEM_ENTITY_API EntityMapLoader : public Resource::ResourceLoader
    {
    public:

        EntityMapLoader();
        void SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry );

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry;
    };
}