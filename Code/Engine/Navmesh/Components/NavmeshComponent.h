#pragma once

#include "../_Module/API.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_ENGINE_NAVMESH_API NavmeshComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;
        friend class NavmeshSystem;

    private:

        EXPOSE TResourcePtr<NavmeshData>        m_pNavmeshData;
    };
}