#include "CameraComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void CameraComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();
        m_viewVolume = Math::ViewVolume( Float2( 1.0f ), TRange<float>( 0.1f, m_depth ), Radians( m_FOV ), GetWorldTransform().ToMatrix() );

        // Decompose current view into heading and pitch
        //-------------------------------------------------------------------------

        auto currentRotation = GetLocalTransform().ToEulerAngles();
        m_heading = currentRotation.m_z;
        m_pitch = currentRotation.m_x;

        // Sanity check: viewport values
        //-------------------------------------------------------------------------

        if( !Math::IsInRangeInclusive( m_viewportTopLeft.m_x, 0.0f, 1.0f ) || !Math::IsInRangeInclusive( m_viewportTopLeft.m_y, 0.0f, 1.0f ) )
        {
            KRG_LOG_WARNING( "Camera", "Invalid viewport top-left position supplied. Clamping values!" );
            m_viewportTopLeft.m_x = Math::Clamp( m_viewportTopLeft.m_x, 0.0f, 1.0f );
            m_viewportTopLeft.m_y = Math::Clamp( m_viewportTopLeft.m_y, 0.0f, 1.0f );
        }

        if ( !Math::IsInRangeInclusive( m_viewportBottomRight.m_x, 0.0f, 1.0f ) || !Math::IsInRangeInclusive( m_viewportBottomRight.m_y, 0.0f, 1.0f ) )
        {
            KRG_LOG_WARNING( "Camera", "Invalid viewport dimensions supplied. Clamping values!" );
            m_viewportBottomRight.m_x = Math::Clamp( m_viewportBottomRight.m_x, 0.0f, 1.0f );
            m_viewportBottomRight.m_y = Math::Clamp( m_viewportBottomRight.m_y, 0.0f, 1.0f );
        }

        if( m_viewportBottomRight.m_x <= m_viewportTopLeft.m_x || m_viewportBottomRight.m_y <= m_viewportTopLeft.m_y )
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
        float const sizeX = viewportDimensions.m_x * newViewDimensions.m_x;
        float const sizeY = viewportDimensions.m_y * newViewDimensions.m_y;
        Float2 const actualViewDimensions( sizeX, sizeY );
        m_viewVolume.SetViewDimensions( actualViewDimensions );
    }

    void CameraComponent::OnWorldTransformUpdated()
    {
        m_viewVolume.SetWorldMatrix( GetWorldTransform().ToMatrix() );
    }
}
