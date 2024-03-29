#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntityDescriptors.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_ENGINE_CORE_API EntityCollectionLoader : public Resource::ResourceLoader
    {
    public:

        EntityCollectionLoader();
        void SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry );

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry;
    };
}