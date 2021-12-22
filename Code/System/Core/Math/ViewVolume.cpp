#include "ViewVolume.h"
#include "System/Core/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    // Taken from DirectXMath: XMMatrixPerspectiveFovRH
    // TODO: Rewrite with KRG math lib
    static Matrix CreatePerspectiveProjectionMatrix( float verticalFOV, float aspectRatio, float nearPlaneZ, float farPlaneZ )
    {
        KRG_ASSERT( nearPlaneZ > 0.f && farPlaneZ > 0.f );
        KRG_ASSERT( !Math::IsNearEqual( verticalFOV, 0.0f, 0.00001f * 2.0f ) && !Math::IsNearEqual( aspectRatio, 0.0f, 0.00001f ) && !Math::IsNearEqual( farPlaneZ, nearPlaneZ, 0.00001f ) );

        Vector sinFov, cosFov;
        Vector::SinCos( sinFov, cosFov, 0.5f * verticalFOV );
        Vector height = cosFov / sinFov;
        float const focalRange = farPlaneZ / ( nearPlaneZ - farPlaneZ );

        Vector vValues( height.GetX() / aspectRatio, height.GetX(), focalRange, focalRange * nearPlaneZ );
        Vector vTemp = _mm_setzero_ps();
        vTemp = _mm_move_ss( vTemp, vValues );

        static __m128 const negativeUnitW = { 0, 0, 0, -1.0f };

        // CosFov / SinFov,0,0,0
        Matrix M;
        M.m_rows[0] = vTemp;
        // 0,Height / AspectRatio,0,0
        vTemp = vValues;
        vTemp = _mm_and_ps( vTemp, SIMD::g_mask0Y00 );
        M.m_rows[1] = vTemp;
        // m_x=fRange,m_y=-fRange * NearZ,0,-1.0f
        vTemp = _mm_setzero_ps();
        vValues = _mm_shuffle_ps( vValues, negativeUnitW, _MM_SHUFFLE( 3, 2, 3, 2 ) );
        // 0,0,fRange,-1.0f
        vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 3, 0, 0, 0 ) );
        M.m_rows[2] = vTemp;
        // 0,0,fRange * NearZ,0.0f
        vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 2, 1, 0, 0 ) );
        M.m_rows[3] = vTemp;
        return M;
    }

    // Taken from DirectXMath: XMMatrixOrthographicRH
    // TODO: Rewrite with KRG math lib
    static Matrix CreateOrthographicProjectionMatrix( float width, float height, float nearPlane, float farPlane )
    {
        KRG_ASSERT( !Math::IsNearEqual( width, 0.0f, 0.00001f ) && !Math::IsNearEqual( height, 0.0f, 0.00001f ) && !Math::IsNearEqual( farPlane, nearPlane, 0.00001f ) );

        float const fRange = 1.0f / ( nearPlane - farPlane );

        Vector vValues = { 2.0f / width, 2.0f / height, fRange, fRange * nearPlane };
        Vector vTemp = _mm_setzero_ps();
        // Copy m_x only
        vTemp = _mm_move_ss( vTemp, vValues );

        Matrix M;
        // 2.0f / ViewWidth,0,0,0
        M.m_rows[0] = vTemp;
        // 0,2.0f / ViewHeight,0,0
        vTemp = vValues;
        vTemp = _mm_and_ps( vTemp, SIMD::g_mask0Y00 );
        M.m_rows[1] = vTemp;
        // m_x=fRange,m_y=fRange * nearPlane,0,1.0f
        vTemp = _mm_setzero_ps();
        vValues = _mm_shuffle_ps( vValues, Vector::UnitW, _MM_SHUFFLE( 3, 2, 3, 2 ) );
        // 0,0,fRange,0.0f
        vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 2, 0, 0, 0 ) );
        M.m_rows[2] = vTemp;
        // 0,0,fRange * nearPlane,1.0f
        vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 3, 1, 0, 0 ) );
        M.m_rows[3] = vTemp;
        return M;
    }

    // Taken from DirectXMath: XMMatrixOrthographicOffCenterRH
    // TODO: Rewrite with KRG math lib
    static Matrix CreateOrthographicProjectionMatrixOffCenter( float left, float right, float bottom, float top, float nearPlane, float farPlane )
    {
        KRG_ASSERT( !Math::IsNearEqual( right, left, 0.00001f ) && !Math::IsNearEqual( top, bottom, 0.00001f ) && !Math::IsNearEqual( farPlane, nearPlane, 0.00001f ) );

        float const fReciprocalWidth = 1.0f / ( right - left );
        float const fReciprocalHeight = 1.0f / ( top - bottom );
        float const fRange = 1.0f / ( nearPlane - farPlane );

        Vector vValues = { fReciprocalWidth, fReciprocalHeight, fRange, 1.0f };
        Vector rMem2 = { -( left + right ), -( top + bottom ), nearPlane, 1.0f };
        Vector vTemp = _mm_setzero_ps();

        // Copy x only
        vTemp = _mm_move_ss( vTemp, vValues );
        // fReciprocalWidth*2,0,0,0
        vTemp = _mm_add_ss( vTemp, vTemp );

        Matrix M;
        M.m_rows[0] = vTemp;
        // 0,fReciprocalHeight*2,0,0
        vTemp = vValues;
        vTemp = _mm_and_ps( vTemp, SIMD::g_mask0Y00 );
        vTemp = _mm_add_ps( vTemp, vTemp );
        M.m_rows[1] = vTemp;
        // 0,0,fRange,0.0f
        vTemp = vValues;
        vTemp = _mm_and_ps( vTemp, SIMD::g_mask00Z0 );
        M.m_rows[2] = vTemp;
        // -(left + right)*fReciprocalWidth,-(top + bottom)*fReciprocalHeight,fRange*-nearPlane,1.0f
        vValues = _mm_mul_ps( vValues, rMem2 );
        M.m_rows[3] = vValues;
        return M;
    }

    // Adapted from DirectXMath: XMMatrixLookAtLH (but changed to be right handed)
    // View matrix is -Z forward, +Y up and +X right
    static Matrix CreateLookToMatrix( Vector const& viewPosition, Vector const& viewDirection, Vector const& upDirection )
    {
        KRG_ASSERT( viewPosition.IsW1() && !viewDirection.IsZero3() && !viewDirection.IsInfinite3() && !upDirection.IsZero3() && !upDirection.IsInfinite3() );

        Vector const R2 = viewDirection.GetNegated().GetNormalized3();
        Vector const R0 = Vector::Cross3( upDirection, R2 ).GetNormalized3();
        Vector const R1 = Vector::Cross3( R2, R0 );

        Vector const NegViewPosition = viewPosition.GetNegated();

        Vector const D0 = Vector::Dot3( R0, NegViewPosition );
        Vector const D1 = Vector::Dot3( R1, NegViewPosition );
        Vector const D2 = Vector::Dot3( R2, NegViewPosition );

        Matrix M;
        M[0] = Vector::Select( D0, R0, Vector::Select1110 );
        M[1] = Vector::Select( D1, R1, Vector::Select1110 );
        M[2] = Vector::Select( D2, R2, Vector::Select1110 );
        M[3] = Vector::UnitW;

        M.Transpose();
        return M;
    }

    KRG_FORCE_INLINE static Matrix CreateLookAtMatrix( Vector const& viewPosition, Vector const& focusPosition, Vector const& upDirection )
    {
        Vector const viewDirection = focusPosition - viewPosition;
        return CreateLookToMatrix( viewPosition, viewDirection, upDirection );
    }

    // Adapted from DirectXMath: XMMatrixLookAtLH (but changed to be right handed)
    // View matrix is -Z forward, +Y up and +X right
    static Matrix CreateLookToMatrix( Vector const& viewPosition, Vector const& viewForward, Vector const& viewRight, Vector const& viewUp )
    {
        Vector const R2 = viewForward.GetNegated();
        Vector const R0 = viewRight;
        Vector const R1 = viewUp;

        Vector const NegViewPosition = viewPosition.GetNegated();

        Vector const D0 = Vector::Dot3( R0, NegViewPosition );
        Vector const D1 = Vector::Dot3( R1, NegViewPosition );
        Vector const D2 = Vector::Dot3( R2, NegViewPosition );

        Matrix M;
        M[0] = Vector::Select( D0, R0, Vector::Select1110 );
        M[1] = Vector::Select( D1, R1, Vector::Select1110 );
        M[2] = Vector::Select( D2, R2, Vector::Select1110 );
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
        : m_viewDimensions( viewDimensions )
        , m_depthRange( depthRange )
        , m_type( ProjectionType::Orthographic )
    {
        KRG_ASSERT( IsValid() );
        CalculateProjectionMatrix();
        SetView( worldMatrix.GetTranslation(), worldMatrix.GetForwardVector(), worldMatrix.GetUpVector() ); // Updates all internals

    }

    ViewVolume::ViewVolume( Float2 const& viewDimensions, FloatRange depthRange, Radians FOV, Matrix const& worldMatrix )
        : m_viewDimensions( viewDimensions )
        , m_FOV( FOV )
        , m_depthRange( depthRange )
        , m_type( ProjectionType::Perspective )
    {
        KRG_ASSERT( IsValid() );
        CalculateProjectionMatrix();
        SetView( worldMatrix.GetTranslation(), worldMatrix.GetForwardVector(), worldMatrix.GetUpVector() ); // Updates all internals
    }

    bool ViewVolume::IsValid() const
    {
        if ( m_viewDimensions.m_x <= 0.0f || m_viewDimensions.m_y <= 0.0f || m_depthRange.m_start < 0.0f || !m_depthRange.IsValid() )
        {
            return false;
        }

        if ( m_type == ProjectionType::Perspective )
        {
            if ( Math::IsNearZero( (float) m_FOV ) )
            {
                return false;
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ViewVolume::SetView( Vector const& position, Vector const& viewDir, Vector const& upDir )
    {
        KRG_ASSERT( viewDir.IsNormalized3() && upDir.IsNormalized3() );

        m_viewPosition = position;
        m_viewForwardDirection = viewDir;
        m_viewRightDirection = Vector::Cross3( viewDir, upDir );
        m_viewUpDirection = Vector::Cross3( m_viewRightDirection, viewDir );

        m_viewMatrix = CreateLookToMatrix( position.GetWithW1(), m_viewForwardDirection, m_viewRightDirection, m_viewUpDirection );
        UpdateInternals();
    }

    void ViewVolume::SetViewDimensions( Float2 dimensions )
    {
        KRG_ASSERT( dimensions.m_x > 0.0f && dimensions.m_y > 0.0f );
        m_viewDimensions = dimensions;
        CalculateProjectionMatrix();
        UpdateInternals();
    }

    void ViewVolume::SetHorizontalFOV( Radians FOV )
    {
        KRG_ASSERT( IsPerspective() && FOV > 0.0f );
        m_FOV = FOV;
        CalculateProjectionMatrix();
        UpdateInternals();
    }

    //-------------------------------------------------------------------------

    void ViewVolume::CalculateProjectionMatrix()
    {
        if ( IsOrthographic() )
        {
            m_projectionMatrix = CreateOrthographicProjectionMatrix( m_viewDimensions.m_x, m_viewDimensions.m_y, m_depthRange.m_start, m_depthRange.m_end );
        }
        else
        {
            Radians const verticalFOV = ConvertHorizontalToVerticalFOV( m_viewDimensions.m_x, m_viewDimensions.m_y, m_FOV );
            m_projectionMatrix = CreatePerspectiveProjectionMatrix( (float) verticalFOV, GetAspectRatio(), m_depthRange.m_start, m_depthRange.m_end );
        }
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
        UpdateInternals();
    }

    void ViewVolume::UpdateInternals()
    {
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
            Vector const fwdDir = GetViewForwardVector();
            Vector const upDir = GetViewUpVector();
            Vector const rightDir = GetViewRightVector();

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
            Vector const fwdDir = GetViewForwardVector();
            Vector const rightDir = GetViewRightVector();
            Vector const upDir = GetViewUpVector();

            // Near/far plane center points
            Vector const centerNear = Vector::MultiplyAdd( fwdDir, Vector( m_depthRange.m_start ), viewPosition );
            Vector const centerFar = Vector::MultiplyAdd( fwdDir, Vector( m_depthRange.m_end ), viewPosition );

            // Get projected viewport extents on near/far planes
            Radians const verticalFOV = ConvertHorizontalToVerticalFOV( m_viewDimensions.m_x, m_viewDimensions.m_y, m_FOV );
            float const aspectRatio = GetAspectRatio();
            float e = Math::Tan( (float) verticalFOV * 0.5f );
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
    void ViewVolume::DrawDebug( Drawing::DrawContext& drawingContext ) const
    {
        Vector corners[8];
        GetCorners( corners );

        Vector const viewPosition = GetViewPosition();
        drawingContext.DrawPoint( viewPosition, Colors::LimeGreen, 10.0f );

        Vector const fwdDir = GetViewForwardVector();
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