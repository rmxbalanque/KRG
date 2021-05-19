#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphLoader final : public Resource::ResourceLoader
    {
    public:

        AnimationGraphLoader();

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
    };
}