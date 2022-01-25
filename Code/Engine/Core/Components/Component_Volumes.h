#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Drawing{ class DrawContext; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API VolumeComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( VolumeComponent );

    public:

        inline VolumeComponent() = default;
        inline VolumeComponent( StringID name ) : SpatialEntityComponent( name ) {}

        #if KRG_DEVELOPMENT_TOOLS
        virtual Color GetVolumeColor() const { return Colors::Gray; }
        virtual void Draw( Drawing::DrawContext& drawingCtx ) const {}
        #endif
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API BoxVolumeComponent : public VolumeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( BoxVolumeComponent );

    public:

        inline BoxVolumeComponent() = default;
        inline BoxVolumeComponent( StringID name ) : VolumeComponent( name ) {}

        // Get the half-size of the volume!
        inline Float3 GetVolumeLocalExtents() const { return GetWorldTransform().GetScale(); }

        #if KRG_DEVELOPMENT_TOOLS
        virtual void Draw( Drawing::DrawContext& drawingCtx ) const override;
        #endif

    protected:

        virtual void Initialize() override;
    };
}