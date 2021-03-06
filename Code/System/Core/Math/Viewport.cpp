#include "Viewport.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        Viewport::Viewport()
            : m_viewVolume( Float2( 1, 1 ), TRange<float>( 0.1f, 1000.0f ) )
        {}

        Viewport::Viewport( Int2 const& topLeft, Int2 const& dimensions, Math::ViewVolume const& viewVolume )
            : m_viewVolume( viewVolume )
            , m_cullingVolume( viewVolume )
            , m_topLeftPosition( topLeft )
            , m_size( dimensions )
        {
            KRG_ASSERT( IsValid() );
        }

        void Viewport::Resize( Int2 const& dimensions, Int2 const& topLeftPosition )
        {
            m_size = Float2( dimensions );
            m_topLeftPosition = Float2( topLeftPosition );
            KRG_ASSERT( IsValid() );
        }

        void Viewport::SetViewVolume( Math::ViewVolume const& viewVolume, Math::ViewVolume const& cullingVolume )
        {
            m_viewVolume = viewVolume;
            m_cullingVolume = cullingVolume;

            // Update the aspect ratio based on the viewport desc we are using
            if ( m_viewVolume.IsPerspective() )
            {
                m_viewVolume.SetViewDimensions( m_size );
                m_cullingVolume.SetViewDimensions( m_size );
            }
        }

        //-------------------------------------------------------------------------

        Vector Viewport::WorldSpaceToClipSpace( Vector const& pointWS ) const
        {
            Vector pointCS = m_viewVolume.GetViewProjectionMatrix().TransformPoint( pointWS.GetAsPoint() );

            Vector const W = pointCS.GetSplatW().Abs();
            if ( !W.IsZero4() )
            {
                pointCS /= W;
            }

            return pointCS;
        }

        Float2 Viewport::ClipSpaceToScreenSpace( Vector const& pointCS ) const
        {
            // Convert from [-1,1] to [0,1]
            float x = ( pointCS.x + 1 ) / 2;
            float y = ( pointCS.y + 1 ) / 2;

            // Invert Y since screen space origin (0,0) is the top left and in CS it is the bottom right
            y = 1.0f - y;

            // Convert to pixels based on viewport dimensions
            Float2 pointSS;
            pointSS.x = x * m_size.x;
            pointSS.y = y * m_size.y;
            return pointSS;
        }

        Vector Viewport::ScreenSpaceToClipSpace( Float2 const& pointSS ) const
        {
            Float4 pointCS( 0.0f, 0.0f, 0.0f, 1.0f );

            // To Normalized pixel space
            pointCS.x = pointSS.x / m_size.x;
            pointCS.y = pointSS.y / m_size.y;

            // Invert Y
            pointCS.y = 1.0f - pointCS.y;

            // Convert from [0,1] to [-1,1]
            pointCS.x = ( pointCS.x * 2 ) - 1.0f;
            pointCS.y = ( pointCS.y * 2 ) - 1.0f;
            return pointCS;
        }

        LineSegment Viewport::ClipSpaceToWorldSpace( Vector const& pointCS ) const
        {
            Vector nearPoint( pointCS.x, pointCS.y, 0.0f, 1.0f );
            Vector farPoint( pointCS.x, pointCS.y, 1.0f, 1.0f );

            Matrix const& invViewProj = m_viewVolume.GetInverseViewProjectionMatrix();
            nearPoint = invViewProj.TransformPoint( nearPoint );
            nearPoint /= nearPoint.GetSplatW();

            farPoint = invViewProj.TransformPoint( farPoint );
            farPoint /= farPoint.GetSplatW();

            return LineSegment( nearPoint, farPoint );
        }

        Vector Viewport::ScreenSpaceToWorldSpaceNearPlane( Vector const& pointSS ) const
        {
            Vector pointCS = ScreenSpaceToClipSpace( pointSS ).MakePoint();
            pointCS.z = 0.0f;
            pointCS = m_viewVolume.GetInverseViewProjectionMatrix().TransformPoint( pointCS );
            pointCS /= pointCS.GetSplatW();
            return pointCS;
        }

        Vector Viewport::ScreenSpaceToWorldSpaceFarPlane( Vector const& pointSS ) const
        {
            Vector pointCS = ScreenSpaceToClipSpace( pointSS ).MakePoint();
            pointCS.z = 1.0f;
            pointCS = m_viewVolume.GetInverseViewProjectionMatrix().TransformPoint( pointCS );
            pointCS /= pointCS.GetSplatW();
            return pointCS;
        }
    }
}