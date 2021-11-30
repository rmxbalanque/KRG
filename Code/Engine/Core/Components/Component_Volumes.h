#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINE_CORE_API VolumeComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( VolumeComponent );

    public:

        inline VolumeComponent() = default;
        inline VolumeComponent( StringID name ) : SpatialEntityComponent( name ) {}
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API BoxVolumeComponent : public VolumeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( BoxVolumeComponent );

    public:

        inline BoxVolumeComponent() = default;
        inline BoxVolumeComponent( StringID name ) : VolumeComponent( name ) {}

    protected:

        virtual void OnWorldTransformUpdated() override
        {
            Transform const& localTransform = GetLocalTransform();
            OBB const newBounds( localTransform.GetTranslation(), localTransform.GetScale(), localTransform.GetRotation() );
            SetLocalBounds( newBounds );
        }
    };
}