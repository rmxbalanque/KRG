#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class SkeletonLoader final : public Resource::ResourceLoader
    {
    public:

        SkeletonLoader();

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
    };
}