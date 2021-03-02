#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxTriangleMesh;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsMesh : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'PHYS' );
        friend class PhysicsMeshCompiler;
        friend class PhysicsMeshLoader;

        KRG_SERIALIZE_NONE();

    public:

        virtual bool IsValid() const override;

        physx::PxTriangleMesh* GetTriangleMesh() const { return m_pTriangleMesh; }

    private:

        physx::PxTriangleMesh*      m_pTriangleMesh = nullptr;
    };
}