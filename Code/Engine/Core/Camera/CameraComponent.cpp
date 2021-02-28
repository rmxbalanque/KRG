#include "CameraComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void CameraComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();
        m_viewVolume = Math::ViewVolume( Float2( 1.0f ), TRange<F32>( 0.1f, m_depth ), Radians( m_FOV ), GetWorldTransform().ToMatrix() );

        // Decompose current view into heading and pitch
        //-------------------------------------------------------------------------

        auto currentRotation = GetLocalTransform().ToEulerAngles();
        m_heading = currentRotation.z;
        m_pitch = currentRotation.x;

        // Sanity check: viewport values
        //-------------------------------------------------------------------------

        if( !Math::IsInRange( m_viewportTopLeft.x, 0.0f, 1.0f ) || !Math::IsInRange( m_viewportTopLeft.y, 0.0f, 1.0f ) )
        {
            KRG_LOG_WARNING( "Camera", "Invalid viewport top-left position supplied. Clamping values!" );
            m_viewportTopLeft.x = Math::Clamp( m_viewportTopLeft.x, 0.0f, 1.0f );
            m_viewportTopLeft.y = Math::Clamp( m_viewportTopLeft.y, 0.0f, 1.0f );
        }

        if ( !Math::IsInRange( m_viewportBottomRight.x, 0.0f, 1.0f ) || !Math::IsInRange( m_viewportBottomRight.y, 0.0f, 1.0f ) )
        {
            KRG_LOG_WARNING( "Camera", "Invalid viewport dimensions supplied. Clamping values!" );
            m_viewportBottomRight.x = Math::Clamp( m_viewportBottomRight.x, 0.0f, 1.0f );
            m_viewportBottomRight.y = Math::Clamp( m_viewportBottomRight.y, 0.0f, 1.0f );
        }

        if( m_viewportBottomRight.x <= m_viewportTopLeft.x || m_viewportBottomRight.y <= m_viewportTopLeft.y )
        {
            KRG_LOG_WARNING( "Camera", "Invalid viewport dimensions supplied. resetting values to fullscreen." );
            m_viewportTopLeft = Float2::Zero;
            m_viewportBottomRight = Float2::One;
        }
    }

    //-------------------------------------------------------------------------

    void CameraComponent::AdjustHeadingAndPitch( Radians headingDelta, Radians pitchDelta )
    {
        // Adjust heading and pitch based on input
        m_heading += headingDelta;
        m_heading.ClampToPositive();

        m_pitch += pitchDelta;
        m_pitch.ClampToPositive();

        auto const rotZ = Quaternion( Float3::UnitZ, m_heading );
        auto const rotX = Quaternion( Float3::UnitX, m_pitch );
        auto const rotTotal = rotX * rotZ;

        auto localTransform = GetLocalTransform();
        localTransform.SetRotation( rotTotal );
        SetLocalTransform( localTransform );
    }

    void CameraComponent::OnViewResized( Int2 newViewDimensions )
    {
        // Calculate the actual space that this camera's viewport covers
        //-------------------------------------------------------------------------
        Float2 const viewportDimensions( m_viewportBottomRight - m_viewportTopLeft );
        F32 const sizeX = viewportDimensions.x * newViewDimensions.x;
        F32 const sizeY = viewportDimensions.y * newViewDimensions.y;
        Float2 const actualViewDimensions( sizeX, sizeY );
        m_viewVolume.SetViewDimensions( actualViewDimensions );
    }

    void CameraComponent::OnWorldTransformUpdated()
    {
        m_viewVolume.SetWorldMatrix( GetWorldTransform().ToMatrix() );
    }
}
