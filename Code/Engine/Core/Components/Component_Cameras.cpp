#include "Component_Cameras.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void CameraComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();
        m_viewVolume = Math::ViewVolume( Float2( 1.0f ), FloatRange( 0.1f, m_depth ), Radians( m_FOV ), GetWorldTransform().ToMatrix() );
    }

    //-------------------------------------------------------------------------

    void CameraComponent::OnWorldTransformUpdated()
    {
        m_viewVolume.SetWorldMatrix( GetWorldTransform().ToMatrix() );
    }

    //-------------------------------------------------------------------------

    static Transform CalculateFreeLookCameraTransform( Vector const& position, Radians yaw, Radians pitch )
    {
        auto const rotZ = Quaternion( Float3::WorldUp, yaw );
        auto const rotX = Quaternion( Float3::WorldRight, pitch );
        auto const rotTotal = rotX * rotZ;
        return Transform( rotTotal, position );
    }

    void FreeLookCameraComponent::AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta )
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
        ResetToDefaults();
    }

    void OrbitCameraComponent::FinalizeCameraPosition()
    {
        Transform const& parentTransform = GetSpatialParentWorldTransform();
        Vector const orbitTarget = parentTransform.GetTranslation() + m_orbitTargetOffset;

        Transform camWorldTransform = m_orbitTransform;
        camWorldTransform.AddTranslationOffset( orbitTarget );
        SetWorldTransform( camWorldTransform );
    }

    void OrbitCameraComponent::AdjustOrbitAngle( Radians deltaHorizontalAngle, Radians deltaVerticalAngle )
    {
        if ( Math::IsNearZero( deltaVerticalAngle ) && Math::IsNearZero( deltaVerticalAngle ) )
        {
            return;
        }

        // Adjust heading and pitch based on input
        m_yaw -= deltaHorizontalAngle;
        m_yaw.ClampPositive360();

        m_pitch += deltaVerticalAngle;
        m_pitch.Clamp( Degrees( -45 ), Degrees( 45 ) );
        
        auto const rotZ = Quaternion( Float3::WorldUp, m_yaw );
        auto const rotX = Quaternion( Float3::WorldRight, m_pitch );
        auto const rotTotal = rotX * rotZ;

        m_orbitTransform.SetRotation( rotTotal );
        UpdateOrbitLocalPosition();
    }

    void OrbitCameraComponent::AdjustOrbitTargetOffset( Float3 newTargetOffset )
    {
        m_orbitTargetOffset = newTargetOffset;
    }

    void OrbitCameraComponent::AdjustOrbitDistance( float newDistance )
    {
        m_orbitDistance = newDistance;
        UpdateOrbitLocalPosition();
    }

    void OrbitCameraComponent::ResetToDefaults()
    {
        m_orbitTargetOffset = m_defaultOrbitTargetOffset;
        m_orbitDistance = m_defaultOrbitDistance;

        m_pitch = m_yaw = 0.0f;
        m_orbitTransform.SetRotation( Quaternion::Identity );
        m_orbitTransform.SetTranslation( Vector::WorldBackward * m_orbitDistance );
    }
}