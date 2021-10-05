#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxConvexMesh.h"
#include "System/Core/Types/StringID.h"

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
        KRG_REGISTER_RESOURCE( 'PMSH', "Physics Mesh" );
        friend class PhysicsMeshCompiler;
        friend class PhysicsMeshLoader;

        KRG_SERIALIZE_MEMBERS( m_physicsMaterialIDs, m_isConvexMesh );

    public:

        virtual bool IsValid() const override;

        //-------------------------------------------------------------------------

        inline bool IsTriangleMesh() const { return !m_isConvexMesh; }
        inline bool IsConvexMesh() const { return m_isConvexMesh; }

        inline uint16 GetNumMaterialsNeeded() const { return (uint16) m_physicsMaterialIDs.size(); }
        inline TInlineVector<StringID, 4> const& GetPhysicsMaterials() const { return m_physicsMaterialIDs; }

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
        TInlineVector<StringID, 4>  m_physicsMaterialIDs;
        bool                        m_isConvexMesh = false;
    };
}