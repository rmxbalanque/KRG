#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_SYSTEM_RENDER_API MaterialLoader : public Resource::ResourceLoader
        {
        public:

            MaterialLoader();

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
            virtual bool Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const final;
        };
    }
}