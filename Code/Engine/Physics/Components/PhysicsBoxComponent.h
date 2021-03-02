#pragma once

#include "../_Module/API.h"
#include "PhysicsShapeComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsBoxComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;

    protected:

        EXPOSE Float3                               m_boxExtents = Float3( 1.0f );
    };
}