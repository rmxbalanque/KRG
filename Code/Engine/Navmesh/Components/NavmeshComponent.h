#pragma once

#include "../_Module/API.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "System/Resource/ResourcePtr.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_ENGINE_NAVMESH_API NavmeshComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( NavmeshComponent );
        friend class NavmeshWorldSystem;

    private:

        KRG_EXPOSE TResourcePtr<NavmeshData>        m_pNavmeshData;
    };
}