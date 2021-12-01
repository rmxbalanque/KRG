#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------
// Camera
//-------------------------------------------------------------------------
// In KRG, cameras respect the KRG coordinate system (i.e. -Y forward, Z-up )
// This means that they look down the forward axis

namespace KRG
{
    class KRG_ENGINE_CORE_API CameraComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( CameraComponent );

        friend class CameraDebugView;

    public:

        enum class ProjectionType
        {
            KRG_REGISTER_ENUM

            Perspective = 0,
            Orthographic
        };

    public:

        inline CameraComponent() = default;

        inline void UpdateViewDimensions( Float2 const& viewDimensions ) { m_viewVolume.SetViewDimensions( viewDimensions ); }
        inline Math::ViewVolume const& GetViewVolume() const { return m_viewVolume; }

        inline void GetDepthRange() const { m_viewVolume.GetDepthRange(); }
        inline void SetDepthRange( FloatRange depthRange ) { m_viewVolume.SetDepthRange( depthRange ); }

        // Set the horizontal field of view
        inline void SetHorizontalFOV( Radians FOV ) { m_viewVolume.SetHorizontalFOV( FOV ); }

    protected:

        using SpatialEntityComponent::SpatialEntityComponent;

        virtual void Initialize() override;
        virtual void OnWorldTransformUpdated() override;

    protected:

        // Initial Camera Settings - These do not change at runtime, if you want the actual settings, query the view volume
        KRG_EXPOSE Degrees              m_FOV = 90.0f;
        KRG_EXPOSE FloatRange           m_depthRange = FloatRange( 0.1f, 500.0f );
        KRG_EXPOSE ProjectionType       m_projectionType = ProjectionType::Perspective;

        // Runtime Data
        Math::ViewVolume                m_viewVolume;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API FreeLookCameraComponent : public CameraComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( FreeLookCameraComponent );

        friend class CameraDebugView;

    public:

        inline FreeLookCameraComponent() = default;
        inline FreeLookCameraComponent( StringID name ) : CameraComponent( name ) {}

        // Perform local adjustments to the camera's heading and pitch
        void AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta );

        // Set the camera world position and world look at target
        void SetPositionAndLookatTarget( Vector const& cameraPosition, Vector const& lookatTarget );

    protected:

        Radians                         m_yaw = 0;
        Radians                         m_pitch = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API OrbitCameraComponent : public CameraComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( OrbitCameraComponent );

        friend class CameraDebugView;

    public:

        inline OrbitCameraComponent() = default;

        virtual void Initialize() override;

        // Update camera position based on the orbit target
        void FinalizeCameraPosition();

        Vector GetCameraRelativeForwardVector() const { return GetWorldTransform().GetForwardVector(); }
        Vector GetCameraRelativeRightVector() const { return GetWorldTransform().GetRightVector(); }
        Vector GetCameraRelativeUpVector() const { return GetWorldTransform().GetUpVector(); }

        void AdjustOrbitAngle( Radians deltaHorizontalAngle, Radians deltaVerticalAngle );
        void AdjustOrbitTargetOffset( Float3 newTargetOffset );
        void AdjustOrbitDistance( float newDistance );

    private:

        void ResetToDefaults();
        
        inline void UpdateOrbitLocalPosition()
        {
            Vector const orbitCameraPosition = m_orbitTransform.GetRotation().RotateVector( Vector::WorldBackward ) * m_orbitDistance;
            m_orbitTransform.SetTranslation( orbitCameraPosition );
        }

    protected:

        KRG_EXPOSE Float3               m_defaultOrbitTargetOffset = Float3::Zero;
        KRG_EXPOSE float                m_defaultOrbitDistance = 2.0f;

        Radians                         m_yaw = 0;
        Radians                         m_pitch = 0;
        Transform                       m_orbitTransform; // The orbit transform (local to the orbit world space point)

        Float3                          m_orbitTargetOffset = Float3::Zero;
        float                           m_orbitDistance = 2.0f;
    };
}