#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysicsScene;

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_PHYSICS_API PhysicsGeometryLoader : public Resource::ResourceLoader
        {
        public:

            PhysicsGeometryLoader();

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override final;
            virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const override final;
        };
    }
}