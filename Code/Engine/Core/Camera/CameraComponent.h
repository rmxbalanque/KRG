#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINE_CORE_API CameraComponent : public SpatialEntityComponent
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

        inline void UpdateViewDimensions( Float2 const& viewDimensions ) { m_viewVolume.SetViewDimensions( viewDimensions ); }
        inline Math::ViewVolume const& GetViewVolume() const { return m_viewVolume; }

        inline void SetDepthRange( FloatRange depthRange ) { m_viewVolume.SetDepthRange( depthRange ); }
        inline void SetHorizontalFOV( Radians FOV ) { m_viewVolume.SetHorizontalFOV( FOV ); }
        inline void AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta );

    protected:

        virtual void Initialize() override;
        virtual void OnWorldTransformUpdated() override final;

    protected:

        // Camera Settings
        KRG_EXPOSE Degrees              m_FOV = 90.0f;
        KRG_EXPOSE float                m_depth = 500.0f;
        KRG_EXPOSE ProjectionType       m_projectionType = ProjectionType::Perspective;

        // Runtime Data
        Math::ViewVolume                m_viewVolume;
        Radians                         m_heading = 0;
        Radians                         m_pitch = 0;
    };
}