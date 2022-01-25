#include "Component_Cameras.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void CameraComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();
        m_viewVolume = Math::ViewVolume( Float2( 1.0f ), m_depthRange, Radians( m_FOV ), GetWorldTransform().ToMatrix() );
    }

    //-------------------------------------------------------------------------

    void CameraComponent::OnWorldTransformUpdated()
    {
        Transform const& worldTransform = GetWorldTransform();
        m_viewVolume.SetView( worldTransform.GetTranslation(), worldTransform.GetForwardVector(), worldTransform.GetUpVector() );
    }

    //-------------------------------------------------------------------------

    static Transform CalculateFreeLookCameraTransform( Vector const& position, Radians yaw, Radians pitch )
    {
        auto const rotZ = Quaternion( Float3::WorldUp, yaw );
        auto const rotX = Quaternion( Float3::WorldRight, pitch );
        auto const rotTotal = rotX * rotZ;
        return Transform( rotTotal, position );
    }

    void FreeLookCameraComponent::AdjustPitchAndYaw( Radians headingDelta, Radians pitchDelta )
    {
        // Adjust heading and pitch based on input
        m_yaw += headingDelta;
        m_yaw.ClampPositive360();

        m_pitch += pitchDelta;
        m_pitch.ClampPositive360();

        Transform const newCameraTransform = CalculateFreeLookCameraTransform( GetLocalTransform().GetTranslation(), m_yaw, m_pitch );
        SetLocalTransform( newCameraTransform );
    }

    void FreeLookCameraComponent::SetPositionAndLookatTarget( Vector const& cameraPosition, Vector const& lookatTarget )
    {
        if ( cameraPosition.IsNearEqual3( lookatTarget ) )
        {
            Transform worldTransform = GetWorldTransform();
            worldTransform.SetTranslation( cameraPosition );
            SetWorldTransform( worldTransform );
        }
        else // If we have a valid target adjust view direction
        {
            Transform const iwt = GetWorldTransform().GetInverse();
            Vector const localCameraPos = iwt.TransformPoint( cameraPosition );
            Vector const localCameraTarget = iwt.TransformPoint( lookatTarget );

            Vector const camForward = GetLocalTransform().GetForwardVector();
            Vector const camNewForward = ( localCameraTarget - localCameraPos ).GetNormalized3();

            Radians const horizontalDelta = Math::GetYawAngleBetweenVectors( camForward, camNewForward );
            Radians const verticalDelta = Math::GetPitchAngleBetweenNormalizedVectors( camForward, camNewForward );

            //-------------------------------------------------------------------------

            m_yaw += horizontalDelta;
            m_yaw.ClampPositive360();

            m_pitch += verticalDelta;
            m_pitch.ClampPositive360();

            Transform const newCameraTransform = CalculateFreeLookCameraTransform( localCameraPos, m_yaw, m_pitch );
            SetLocalTransform( newCameraTransform );
        }
    }

    //-------------------------------------------------------------------------

    void OrbitCameraComponent::Initialize()
    {
        CameraComponent::Initialize();
        ResetCamera();
    }

    void OrbitCameraComponent::FinalizeCameraPosition()
    {
        Transform const& parentTransform = GetSpatialParentWorldTransform();
        Transform camWorldTransform = m_orbitTransform;

        // Rotation offset is relative to camera to avoid pops when target rotates
        Vector const orbitTarget = parentTransform.GetTranslation() + camWorldTransform.RotateVector( m_orbitTargetOffset );

        camWorldTransform.AddTranslation( orbitTarget );
        SetWorldTransform( camWorldTransform );
    }

    void OrbitCameraComponent::AdjustOrbitAngle( Radians deltaHorizontalAngle, Radians deltaVerticalAngle )
    {
        if ( Math::IsNearZero( (float) deltaVerticalAngle ) && Math::IsNearZero( (float) deltaVerticalAngle ) )
        {
            return;
        }

        // Adjust heading and pitch based on input
        m_yaw -= deltaHorizontalAngle;
        m_yaw.ClampPositive360();

        m_pitch += deltaVerticalAngle;
        m_pitch.Clamp( Degrees( -45 ), Degrees( 45 ) );

        CalculateCameraTransform();
    }

    void OrbitCameraComponent::AdjustOrbitTargetOffset( Float3 newTargetOffset )
    {
        m_orbitTargetOffset = newTargetOffset;
    }

    void OrbitCameraComponent::AdjustOrbitDistance( float newDistance )
    {
        m_orbitDistance = newDistance;
        Vector const newCameraPosition = m_orbitTransform.GetRotation().RotateVector( Vector::WorldBackward ) * m_orbitDistance;
        m_orbitTransform.SetTranslation( newCameraPosition );
    }

    void OrbitCameraComponent::ResetCamera()
    {
        m_orbitTargetOffset = m_defaultOrbitTargetOffset;
        m_orbitDistance = m_defaultOrbitDistance;

        // Align to parent orientation
        if ( HasSpatialParent() )
        {
            Transform const& parentWorldTransform = GetSpatialParentWorldTransform();
            m_yaw = parentWorldTransform.GetRotation().ToEulerAngles().GetYaw();
            m_pitch = 0.0f;

        }
        else // Just align to world forward
        {
            m_pitch = m_yaw = 0.0f;
        }

        CalculateCameraTransform();
    }

    void OrbitCameraComponent::CalculateCameraTransform()
    {
        auto const rotZ = Quaternion( Float3::WorldUp, m_yaw );
        auto const rotX = Quaternion( Float3::WorldRight, m_pitch );
        auto const rotTotal = rotX * rotZ;

        Vector const orbitCameraPosition = rotTotal.RotateVector( Vector::WorldBackward ) * m_orbitDistance;
        m_orbitTransform = Transform( rotTotal, orbitCameraPosition );
    }
}