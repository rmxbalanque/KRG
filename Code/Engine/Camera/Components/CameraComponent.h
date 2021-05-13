#pragma once

#include "../_Module/API.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------

namespace KRG::Camera
{
    class KRG_ENGINE_CAMERA_API CameraComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( CameraComponent );

    public:

        enum class ProjectionType
        {
            KRG_REGISTER_ENUM

            Perspective = 0,
            Orthographic
        };

    public:

        inline CameraComponent() = default;
        inline CameraComponent( StringID name ) : SpatialEntityComponent( UUID::GenerateID(), name ) {}

        Math::ViewVolume const& CalculateViewVolume( Float2 const& viewDimensions );

        inline void SetDepthRange( TRange<float> depthRange ) { m_viewVolume.SetDepthRange( depthRange ); }
        inline void SetHorizontalFOV( Radians FOV ) { m_viewVolume.SetHorizontalFOV( FOV ); }
        inline void AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta );

    protected:

        virtual void Initialize() override;
        virtual void OnWorldTransformUpdated() override final;

    protected:

        // Camera Settings
        EXPOSE Degrees              m_FOV = 90.0f;
        EXPOSE float                m_depth = 500.0f;
        EXPOSE ProjectionType       m_projectionType = ProjectionType::Perspective;

        // Runtime Data
        Math::ViewVolume            m_viewVolume;
        Radians                     m_heading = 0;
        Radians                     m_pitch = 0;
    };
}