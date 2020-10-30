#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_SYSTEM_ANIMATION_API AnimationLoader : public Resource::ResourceLoader
        {
        public:

            AnimationLoader();

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
            virtual bool Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const final;
        };
    }
}