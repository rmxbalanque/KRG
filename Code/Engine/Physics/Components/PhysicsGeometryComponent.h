#pragma once

#include "../_Module/API.h"
#include "System/Physics/PhysicsGeometry.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxActor;
}

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysicsGeometry;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_PHYSICS_API PhysicsGeometryComponent : public SpatialEntityComponent
        {
            KRG_REGISTER_ENTITY_COMPONENT;

            friend class PhysicsWorld;

        protected:

            EXPOSE TResourcePtr<PhysicsGeometry>        m_pPhysicsGeometry;

        private:

            physx::PxActor*                             m_pPhysicsActor = nullptr;
        };
    }
}