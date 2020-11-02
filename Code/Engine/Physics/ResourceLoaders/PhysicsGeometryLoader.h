#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace physx { class PxPhysics; }

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsGeometryLoader : public Resource::ResourceLoader
    {
    public:

        PhysicsGeometryLoader();
        ~PhysicsGeometryLoader() { KRG_ASSERT( m_pPhysics == nullptr ); }

        void SetPhysics( physx::PxPhysics* pPhysics );
        void ClearPhysics() { m_pPhysics = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override final;
        virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const override final;

    private:

        physx::PxPhysics* m_pPhysics = nullptr;
    };
}