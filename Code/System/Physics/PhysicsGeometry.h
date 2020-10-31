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
    class KRG_SYSTEM_PHYSICS_API PhysicsGeometry : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'PHYS' );
        friend class PhysicsGeometryCompiler;
        friend class PhysicsGeometryLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_hasTriangleMeshData ) );

    public:

        virtual bool IsValid() const override;

        physx::PxTriangleMesh* GetTriangleMesh() const { return m_pTriangleMesh; }

    private:

        physx::PxTriangleMesh*      m_pTriangleMesh = nullptr;
        bool                        m_hasTriangleMeshData = false;
    };
}