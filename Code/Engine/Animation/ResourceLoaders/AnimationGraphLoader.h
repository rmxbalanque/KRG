#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphLoader final : public Resource::ResourceLoader
    {
    public:

        AnimationGraphLoader();

        inline void SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry ) { KRG_ASSERT( pTypeRegistry != nullptr ); m_pTypeRegistry = pTypeRegistry; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override;
        virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const override;

        virtual bool Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const override;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry = nullptr;
    };
}