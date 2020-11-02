#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class SkeletonLoader : public Resource::ResourceLoader
        {
        public:

            SkeletonLoader();

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
        };
    }
}