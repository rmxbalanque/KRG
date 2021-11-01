#pragma once

#include "Engine/Render/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API LightComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( LightComponent );
        friend class RenderDebugView;

    public:

        KRG_FORCE_INLINE float GetLightIntensity() const { return m_intensity; }
        KRG_FORCE_INLINE Color const& GetLightColor() const { return m_color; }
        KRG_FORCE_INLINE Vector const& GetLightPosition() const { return GetWorldTransform().GetTranslation(); }

    private:

        KRG_EXPOSE Color                m_color = Colors::White;
        KRG_EXPOSE float                m_intensity = 1.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API DirectionalLightComponent: public LightComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( DirectionalLightComponent );
        friend class RenderDebugView;

    public:

        KRG_FORCE_INLINE Vector GetLightDirection() const { return GetWorldTransform().GetForwardVector(); }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API PointLightComponent : public LightComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PointLightComponent );
        friend class RenderDebugView;

    private:

        // TODO: Fall-off parameters
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API SpotLightComponent : public LightComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( SpotLightComponent );
        friend class RenderDebugView;

    public:

        KRG_FORCE_INLINE float GetLightUmbraAngle() const { return m_umbraAngle; }
        KRG_FORCE_INLINE Vector GetLightDirection() const { return GetWorldTransform().GetForwardVector(); }

    private:

        KRG_EXPOSE Degrees              m_umbraAngle = 45.0f;

        // TODO: Fall-off parameters
    };
}