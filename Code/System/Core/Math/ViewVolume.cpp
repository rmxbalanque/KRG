#include "ViewVolume.h"
#include "System/Core/Debug/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    // Creates a Y-up view matrix for rendering from a Z-up world transform
    static Matrix CalculateViewMatrix( Matrix const& worldTransform )
    {
        Vector const& eyePos = worldTransform.GetTranslationWithW();
        Vector const& eyeDir = worldTransform.GetForwardVector();
        Vector const& upDir = worldTransform.GetAxisZ();

        KRG_ASSERT( eyePos.IsW1() && !eyeDir.IsZero3() && !eyeDir.IsInfinite3() && !upDir.IsZero3() && !upDir.IsInfinite3() );

        Vector const forward = eyeDir.GetNormalized3().GetNegated();
        Vector const right = Vector::Cross3( upDir, forward ).GetNormalized3();
        Vector const up = Vector::Cross3( forward, right );

        Vector NegEyePosition = eyePos.GetNegated();

        Vector const D0 = Vector::Dot3( right, NegEyePosition );
        Vector const D1 = Vector::Dot3( up, NegEyePosition );
        Vector const D2 = Vector::Dot3( forward, NegEyePosition );

        static Vector const mask( 1, 1, 1, 0 );

        Matrix M;
        M[0] = Vector::Select( D0, right, mask );
        M[1] = Vector::Select( D1, up, mask );
        M[2] = Vector::Select( D2, forward, mask );
        M[3] = Vector::UnitW;

        M.Transpose();
        return M;
    }

    // Calculate the 6 planes enclosing the volume
    static void CalculateViewPlanes( Matrix const& viewProjectionMatrix, Plane viewPlanes[6] )
    {
        // Left clipping plane
        viewPlanes[0].a = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
        viewPlanes[0].b = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
        viewPlanes[0].c = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
        viewPlanes[0].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];

        // Right clipping plane
        viewPlanes[1].a = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
        viewPlanes[1].b = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
        viewPlanes[1].c = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
        viewPlanes[1].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];

        // Bottom clipping plane
        viewPlanes[3].a = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
        viewPlanes[3].b = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
        viewPlanes[3].c = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
        viewPlanes[3].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];

        // Top clipping plane
        viewPlanes[2].a = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
        viewPlanes[2].b = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
        viewPlanes[2].c = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
        viewPlanes[2].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];

        // Near clipping plane
        viewPlanes[4].a = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
        viewPlanes[4].b = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
        viewPlanes[4].c = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
        viewPlanes[4].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];

        // Far clipping plane
        viewPlanes[5].a = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
        viewPlanes[5].b = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
        viewPlanes[5].c = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
        viewPlanes[5].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

        // Normalize planes
        viewPlanes[0].Normalize();
        viewPlanes[1].Normalize();
        viewPlanes[2].Normalize();
        viewPlanes[3].Normalize();
        viewPlanes[4].Normalize();
        viewPlanes[5].Normalize();
    }

    //-------------------------------------------------------------------------

    ViewVolume::ViewVolume( Float2 const& viewDimensions, FloatRange depthRange, Matrix const& worldMatrix )
        : m_worldMatrix( worldMatrix )
        , m_viewDimensions( viewDimensions )
        , m_depthRange( depthRange )
        , m_type( ProjectionType::Orthographic )
    {
        KRG_ASSERT( IsValid() );
        CalculateProjectionMatrix();
    }

    ViewVolume::ViewVolume( Float2 const& viewDimensions, FloatRange depthRange, Radians FOV, Matrix const& worldMatrix )
        : m_worldMatrix( worldMatrix )
        , m_viewDimensions( viewDimensions )
        , m_FOV( FOV )
        , m_depthRange( depthRange )
        , m_type( ProjectionType::Perspective )
    {
        KRG_ASSERT( IsValid() );
        CalculateProjectionMatrix();
    }

    bool ViewVolume::IsValid() const
    {
        if ( m_viewDimensions.m_x <= 0.0f || m_viewDimensions.m_y <= 0.0f || m_depthRange.m_start <= 0.0f || !m_depthRange.IsValid() )
        {
            return false;
        }

        if ( m_type == ProjectionType::Perspective )
        {
            if ( Math::IsNearZero( m_FOV ) )
            {
                return false;
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ViewVolume::SetView( Float3 const& position, Float3 const& viewDir, Float3 const& upDir )
    {
        Vector const vDir = ( Vector( viewDir ) - Vector( position ) ).Normalize3();
        Vector vUp = Vector( upDir ).Normalize3();
        Vector const vRight = Vector::Cross3( vDir, vUp ).Normalize3();
        vUp = Vector::Cross3( vRight, vDir ).Normalize3();

        m_worldMatrix[0] = vRight;
        m_worldMatrix[1] = vDir;
        m_worldMatrix[2] = vUp;
        m_worldMatrix[3] = Vector( position, 1.0f );

        UpdateInternals();
    }

    void ViewVolume::SetViewDimensions( Float2 dimensions )
    {
        KRG_ASSERT( dimensions.m_x > 0.0f && dimensions.m_y > 0.0f );
        m_viewDimensions = dimensions;
        CalculateProjectionMatrix();
    }

    void ViewVolume::SetHorizontalFOV( Radians FOV )
    {
        KRG_ASSERT( IsPerspective() && FOV > 0.0f );
        m_FOV = FOV;
        CalculateProjectionMatrix();
    }

    //-------------------------------------------------------------------------

    void ViewVolume::CalculateProjectionMatrix()
    {
        if ( IsOrthographic() )
        {
            m_projectionMatrix = Matrix::OrthographicProjectionMatrix( m_viewDimensions.m_x, m_viewDimensions.m_y, m_depthRange.m_start, m_depthRange.m_end );
        }
        else
        {
            Radians const verticalFOV = ConvertHorizontalToVerticalFOV( m_viewDimensions.m_x, m_viewDimensions.m_y, m_FOV );
            m_projectionMatrix = Matrix::PerspectiveProjectionMatrix( verticalFOV, GetAspectRatio(), m_depthRange.m_start, m_depthRange.m_end );
        }

        UpdateInternals();
    }

    void ViewVolume::SetWorldMatrix( Matrix const& worldTransform )
    {
        m_worldMatrix = worldTransform;
        UpdateInternals();
    }

    OBB ViewVolume::GetOBB() const
    {
        KRG_UNIMPLEMENTED_FUNCTION();
        return OBB();
    }

    void ViewVolume::SetDepthRange( FloatRange depthRange )
    {
        KRG_ASSERT( depthRange.m_start > 0 && depthRange.m_end > 0 && depthRange.IsValid() );
        m_depthRange = depthRange;
        CalculateProjectionMatrix();
    }

    void ViewVolume::UpdateInternals()
    {
        m_viewMatrix = CalculateViewMatrix( m_worldMatrix );
        m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
        m_inverseViewProjectionMatrix = m_viewProjectionMatrix.GetInverse();
        CalculateViewPlanes( m_viewProjectionMatrix, m_viewPlanes );
    }

    //-------------------------------------------------------------------------

    void ViewVolume::GetCorners( Vector corners[8] ) const
    {
        if ( IsOrthographic() )
        {
            Vector const viewPosition = GetViewPosition();
            Vector const fwdDir = GetForwardVector();
            Vector const upDir = GetUpVector();
            Vector const rightDir = GetRightVector();

            Vector const nearPlaneDepthCenterPoint = Vector::MultiplyAdd( fwdDir, Vector( m_depthRange.m_end ), viewPosition );
            Vector const halfWidth( m_viewDimensions.m_x / 2, m_viewDimensions.m_y / 2, 0.0f, 0.0f );

            corners[0] = nearPlaneDepthCenterPoint - ( rightDir * ( m_viewDimensions.m_x / 2 ) ) - ( upDir * ( m_viewDimensions.m_y / 2 ) );
            corners[1] = nearPlaneDepthCenterPoint - ( rightDir * ( m_viewDimensions.m_x / 2 ) ) + ( upDir * ( m_viewDimensions.m_y / 2 ) );
            corners[2] = nearPlaneDepthCenterPoint + ( rightDir * ( m_viewDimensions.m_x / 2 ) ) + ( upDir * ( m_viewDimensions.m_y / 2 ) );
            corners[3] = nearPlaneDepthCenterPoint + ( rightDir * ( m_viewDimensions.m_x / 2 ) ) - ( upDir * ( m_viewDimensions.m_y / 2 ) );

            Vector const farPlaneDepthOffset = fwdDir * m_depthRange.m_end;
            corners[4] = corners[0] + farPlaneDepthOffset;
            corners[5] = corners[1] + farPlaneDepthOffset;
            corners[6] = corners[2] + farPlaneDepthOffset;
            corners[7] = corners[3] + farPlaneDepthOffset;
        }
        else
        {
            Vector const viewPosition = GetViewPosition();
            Vector const fwdDir = GetForwardVector();
            Vector const rightDir = GetRightVector();
            Vector const upDir = GetUpVector();

            // Near/far plane center points
            Vector const centerNear = Vector::MultiplyAdd( fwdDir, Vector( m_depthRange.m_start ), viewPosition );
            Vector const centerFar = Vector::MultiplyAdd( fwdDir, Vector( m_depthRange.m_end ), viewPosition );

            // Get projected viewport extents on near/far planes
            Radians const verticalFOV = ConvertHorizontalToVerticalFOV( m_viewDimensions.m_x, m_viewDimensions.m_y, m_FOV );
            float const aspectRatio = GetAspectRatio();
            float e = Math::Tan( verticalFOV * 0.5f );
            float extentUpNear = e * m_depthRange.m_start;
            float extentRightNear = extentUpNear * aspectRatio;
            float extentUpFar = e * m_depthRange.m_end;
            float extentRightFar = extentUpFar * aspectRatio;

            // Points are just offset from the center points along camera basis
            corners[0] = centerNear - rightDir * extentRightNear - upDir * extentUpNear;
            corners[1] = centerNear - rightDir * extentRightNear + upDir * extentUpNear;
            corners[2] = centerNear + rightDir * extentRightNear + upDir * extentUpNear;
            corners[3] = centerNear + rightDir * extentRightNear - upDir * extentUpNear;
            corners[4] = centerFar - rightDir * extentRightFar - upDir * extentUpFar;
            corners[5] = centerFar - rightDir * extentRightFar + upDir * extentUpFar;
            corners[6] = centerFar + rightDir * extentRightFar + upDir * extentUpFar;
            corners[7] = centerFar + rightDir * extentRightFar - upDir * extentUpFar;
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void ViewVolume::DrawDebug( Debug::DrawingContext& drawingContext ) const
    {
        Vector corners[8];
        GetCorners( corners );

        Vector const viewPosition = GetViewPosition();
        drawingContext.DrawPoint( viewPosition, Colors::LimeGreen, 10.0f );

        Vector const fwdDir = GetForwardVector();
        drawingContext.DrawArrow( viewPosition, viewPosition + ( fwdDir * m_depthRange.m_end ), Colors::Pink, 3.0f );

        //-------------------------------------------------------------------------

        drawingContext.DrawLine( corners[0], corners[4], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[1], corners[5], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[2], corners[6], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[3], corners[7], Colors::Cyan, 3.0f );

        drawingContext.DrawLine( corners[0], viewPosition, Colors::Red, 1.5f );
        drawingContext.DrawLine( corners[1], viewPosition, Colors::Red, 1.5f );
        drawingContext.DrawLine( corners[2], viewPosition, Colors::Red, 1.5f );
        drawingContext.DrawLine( corners[3], viewPosition, Colors::Red, 1.5f );

        //-------------------------------------------------------------------------

        drawingContext.DrawPoint( corners[0], Colors::Cyan, 10.0f );
        drawingContext.DrawPoint( corners[1], Colors::Cyan, 10.0f );
        drawingContext.DrawPoint( corners[2], Colors::Cyan, 10.0f );
        drawingContext.DrawPoint( corners[3], Colors::Cyan, 10.0f );

        drawingContext.DrawLine( corners[0], corners[1], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[1], corners[2], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[2], corners[3], Colors::Cyan, 3.0f );
        drawingContext.DrawLine( corners[3], corners[0], Colors::Cyan, 3.0f );

        //-------------------------------------------------------------------------

        drawingContext.DrawPoint( corners[4], Colors::Yellow, 10.0f );
        drawingContext.DrawPoint( corners[5], Colors::Yellow, 10.0f );
        drawingContext.DrawPoint( corners[6], Colors::Yellow, 10.0f );
        drawingContext.DrawPoint( corners[7], Colors::Yellow, 10.0f );

        drawingContext.DrawLine( corners[4], corners[5], Colors::Yellow, 3.0f );
        drawingContext.DrawLine( corners[5], corners[6], Colors::Yellow, 3.0f );
        drawingContext.DrawLine( corners[6], corners[7], Colors::Yellow, 3.0f );
        drawingContext.DrawLine( corners[7], corners[4], Colors::Yellow, 3.0f );
    }
    #endif

    //-------------------------------------------------------------------------

    ViewVolume::IntersectionResult ViewVolume::Intersect( AABB const& aabb ) const
    {
        Vector const center( aabb.GetCenter() );
        Vector const extents( aabb.GetExtents() );

        auto result = 0; // inside
        for ( auto i = 0u; i < 6; i++ )
        {
            Plane plane( m_viewPlanes[i] );
            Vector const absPlane = m_viewPlanes[i].ToVector().Abs();

            auto const distance = plane.SignedDistanceToPoint( center );
            auto const radius = Vector::Dot3( extents, absPlane );

            // Is outside
            if ( ( distance + radius ).IsLessThan4( Vector::Zero ) )
            {
                return IntersectionResult::FullyOutside;
            }

            // Intersects
            if ( ( distance - radius ).IsLessThan4( Vector::Zero ) )
            {
                return IntersectionResult::Intersects;
            }
        }

        return IntersectionResult::FullyInside;
    }

    ViewVolume::IntersectionResult ViewVolume::Intersect( Vector const& point ) const
    {
        auto result = 0; // inside
        for ( auto i = 0u; i < 6; i++ )
        {
            Plane plane( m_viewPlanes[i] );

            Vector const distance = plane.SignedDistanceToPoint( point );

            // Is outside
            if ( distance.IsLessThan4( Vector::Zero ) )
            {
                return IntersectionResult::FullyOutside;
            }
        }

        return IntersectionResult::FullyInside;
    }
}