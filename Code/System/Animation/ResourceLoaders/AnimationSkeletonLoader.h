#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_SYSTEM_ANIMATION_API SkeletonLoader : public Resource::ResourceLoader
        {
        public:

            SkeletonLoader();

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryArchive& archive ) const final;
        };
    }
}