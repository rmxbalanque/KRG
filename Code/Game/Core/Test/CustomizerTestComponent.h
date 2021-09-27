#pragma once

#include "../_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "Engine/Core/Entity/EntityComponent.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Transform.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_GAME_CORE_API CustomizerTestComponent : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( CustomizerTestComponent );

        friend class CustomizerTestSystem;

    public:

        CustomizerTestComponent() = default;

    protected:

        KRG_EXPOSE TVector<ResourceID>  m_meshIDs;
        KRG_EXPOSE TVector<ResourceID>  m_armorMeshIDs;
        KRG_EXPOSE TVector<ResourceID>  m_hairMeshIDs;
        KRG_EXPOSE TVector<ResourceID>  m_materialIDs;
        KRG_EXPOSE TVector<ResourceID>  m_animationIDs;
        KRG_EXPOSE ResourceID           m_skeletonID;
    };
}