#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxConvexMesh.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxTriangleMesh;
    class PxConvexMesh;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsMesh : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'PMSH' );
        friend class PhysicsMeshCompiler;
        friend class PhysicsMeshLoader;

        KRG_SERIALIZE_MEMBERS( m_numMaterialsNeeded, m_isConvexMesh );

    public:

        virtual bool IsValid() const override;

        //-------------------------------------------------------------------------

        inline bool IsTriangleMesh() const { return !m_isConvexMesh; }
        inline bool IsConvexMesh() const { return m_isConvexMesh; }

        inline uint16 GetNumMaterialsNeeded() const { KRG_ASSERT( IsTriangleMesh() ); return m_numMaterialsNeeded; }

        //-------------------------------------------------------------------------

        inline physx::PxTriangleMesh const* GetTriangleMesh() const
        {
            KRG_ASSERT( !m_isConvexMesh );
            return m_pMesh->is<physx::PxTriangleMesh>(); 
        }
        
        inline physx::PxConvexMesh const* GetConvexMesh() const 
        {
            KRG_ASSERT( m_isConvexMesh );
            return m_pMesh->is<physx::PxConvexMesh>();
        }

    private:

        physx::PxBase*              m_pMesh = nullptr;
        uint16                      m_numMaterialsNeeded = 0; // Only relevant for triangle meshes
        bool                        m_isConvexMesh = false;
    };
}