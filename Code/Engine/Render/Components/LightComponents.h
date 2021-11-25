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
        KRG_FORCE_INLINE bool GetShadowed() const { return m_shadowed; }

    private:

        KRG_EXPOSE Color                m_color = Colors::White;
        KRG_EXPOSE float                m_intensity = 1.0f;
        KRG_EXPOSE bool                 m_shadowed = false;
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

    public:

        KRG_FORCE_INLINE float GetLightRadius() const { return m_radius; }

    private:

        KRG_EXPOSE float                m_radius = 1.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API SpotLightComponent : public LightComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( SpotLightComponent );
        friend class RenderDebugView;

    public:

        KRG_FORCE_INLINE float GetLightRadius() const { return m_radius; }
        KRG_FORCE_INLINE Degrees GetLightInnerUmbraAngle() const { return m_innerUmbraAngle; }
        KRG_FORCE_INLINE Degrees GetLightOuterUmbraAngle() const { return m_outerUmbraAngle; }
        KRG_FORCE_INLINE Vector GetLightDirection() const { return GetWorldTransform().GetForwardVector(); }

    private:

        KRG_EXPOSE Degrees              m_innerUmbraAngle = 0.0f;
        KRG_EXPOSE Degrees              m_outerUmbraAngle = 45.0f;
        KRG_EXPOSE float                m_radius = 1.0f;

        // TODO: Fall-off parameters
    };
}