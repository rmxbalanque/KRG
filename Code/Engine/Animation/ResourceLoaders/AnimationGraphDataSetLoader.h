#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphDataSetLoader final : public Resource::ResourceLoader
    {
    public:

        AnimationGraphDataSetLoader();

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const;
        virtual bool Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const;
    };
}