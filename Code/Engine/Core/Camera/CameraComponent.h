#pragma once

#include "../_Module/API.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINE_CORE_API CameraComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

    public:

        enum class ProjectionType
        {
            KRG_REGISTER_ENUM

            Perspective = 0,
            Orthographic
        };

    public:

        inline Math::ViewVolume const& GetViewVolume() const { return m_viewVolume; }
        inline Math::Rectangle GetViewportRect() const { return Math::Rectangle( m_viewportTopLeft, m_viewportBottomRight - m_viewportTopLeft ); }

        inline void SetDepthRange( TRange<F32> depthRange ) { m_viewVolume.SetDepthRange( depthRange ); }
        inline void SetHorizontalFOV( Radians FOV ) { m_viewVolume.SetHorizontalFOV( FOV ); }
        inline void AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta );
        inline void OnViewResized( Int2 newViewDimensions );

    protected:

        virtual void Initialize() override;
        virtual void OnWorldTransformUpdated() override final;

    protected:

        // Camera Settings
        EXPOSE Degrees              m_FOV = 90.0f;
        EXPOSE F32                  m_depth = 500.0f;
        EXPOSE ProjectionType       m_projectionType = ProjectionType::Perspective;

        // The size of this camera's viewport (in normalized space)
        EXPOSE Float2               m_viewportTopLeft = Float2::Zero;
        EXPOSE Float2               m_viewportBottomRight = Float2::One;

        // Runtime Data
        Math::ViewVolume            m_viewVolume;
        Radians                     m_heading = 0;
        Radians                     m_pitch = 0;
    };
}