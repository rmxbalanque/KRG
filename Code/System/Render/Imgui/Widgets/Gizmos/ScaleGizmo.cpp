#include "ScaleGizmo.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    static float const g_axisLength = 75.0f;
    static float const g_axisThickness = 5.0f;
    static float const g_originSphereRadius = 8.0f;
    static float const g_selectedOriginSphereRadius = 12.0f;
    static float const g_originSphereAxisOffset = g_originSphereRadius + 8.0f;
    static Color const g_selectedColor = Colors::Yellow;
    static Color const g_planeColor = Colors::Gray;

    //-------------------------------------------------------------------------

    // Returns true is mouse is hovering over the origin
    bool ScalingGizmo::DrawOrigin( Vector const& originPosition )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        uint32 const originColor = ConvertColor( ( m_mode == ManipulationMode::ScaleXYZ || m_isOriginHovered ) ? g_selectedColor : Colors::White );

        pDrawList->AddCircleFilled( originPosition.ToFloat2(), 3.0f, originColor, 20 );
        pDrawList->AddCircle( originPosition.ToFloat2(), 8.0f, originColor, 20, 2.0f );

        if ( m_isOriginHovered || m_mode == ManipulationMode::ScaleXYZ )
        {
            pDrawList->AddCircle( originPosition.ToFloat2(), g_selectedOriginSphereRadius, originColor, 12, 2.0f );
        }

        //-------------------------------------------------------------------------

        Vector const mousePos = io.MousePos;
        float const mouseToOriginDistance = originPosition.GetDistance2( mousePos );
        bool const isHovered = mouseToOriginDistance < g_originSphereRadius;
        return isHovered;
    }

    // Returns true is mouse is hovering over the axis
    bool ScalingGizmo::DrawScaleAxis( Vector const& start, Vector const& end, Color color )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        Vector const axisDirection = ( end - start ).GetNormalized2();
        if ( axisDirection.IsNearZero4() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        Vector const offsetStart = start + ( axisDirection * g_originSphereAxisOffset );
        uint32 const axisColor = ConvertColor( color );
        pDrawList->AddLine( offsetStart.ToFloat2(), end.ToFloat2(), axisColor, g_axisThickness );
        pDrawList->AddCircleFilled( end.ToFloat2(), g_axisThickness * 1.5, axisColor );

        //-------------------------------------------------------------------------

        LineSegment const axisLine( offsetStart, end );
        Vector const mousePos = io.MousePos;
        float const mouseToLineDistance = axisLine.GetDistanceBetweenSegmentAndPoint( mousePos );
        bool const isHovered = mouseToLineDistance < ( ( g_axisThickness / 2 ) + 3.0f );

        return isHovered;
    }

    // Returns true is mouse is hovering over the widget
    bool ScalingGizmo::DrawPlaneManipulationWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        auto planeAxisDeltaNearX = ( axis0 * ( g_axisLength * 0.0f + g_originSphereAxisOffset ) );
        auto planeAxisDeltaFarX = ( axis0 * ( g_axisLength * 0.4f + g_originSphereAxisOffset ) );
        auto planeAxisDeltaNearY = ( axis1 * ( g_axisLength * 0.0f + g_originSphereAxisOffset ) );
        auto planeAxisDeltaFarY = ( axis1 * ( g_axisLength * 0.4f + g_originSphereAxisOffset ) );

        ImVec2 manipulationQuadVerts[] =
        {
            ( origin + planeAxisDeltaFarX + planeAxisDeltaNearY ).ToFloat2(),
            ( origin + planeAxisDeltaNearX + planeAxisDeltaNearY ).ToFloat2(),
            ( origin + planeAxisDeltaNearX + planeAxisDeltaFarY ).ToFloat2(),
        };

        pDrawList->AddPolyline( manipulationQuadVerts, 3, ConvertColor( color ), true, 1.0f );
        pDrawList->AddConvexPolyFilled( manipulationQuadVerts, 3, ConvertColor( color.GetAlphaVersion( 0.75f ) ) );

        //-------------------------------------------------------------------------

        Vector const mousePos = io.MousePos;
        auto LeftHandTest = [&mousePos, &manipulationQuadVerts] ( int32 side0, int32 side1 )
        {
            float D = ( manipulationQuadVerts[side1].x - manipulationQuadVerts[side0].x ) * ( mousePos.m_y - manipulationQuadVerts[side0].y ) - ( mousePos.m_x - manipulationQuadVerts[side0].x ) * ( manipulationQuadVerts[side1].y - manipulationQuadVerts[side0].y );
            return D > 0;
        };

        bool const allInsideClockwise = LeftHandTest( 0, 1 ) && LeftHandTest( 1, 2 ) && LeftHandTest( 2, 0 );
        bool const allInsideCounterClockwise = LeftHandTest( 0, 2 ) && LeftHandTest( 2, 1 ) && LeftHandTest( 1, 0 );
        bool const isHovered = allInsideClockwise || allInsideCounterClockwise;
        return isHovered;
    }

    //-------------------------------------------------------------------------

    void ScalingGizmo::UpdateMode( Render::Viewport const& viewport )
    {
        ImGuiIO& io = ImGui::GetIO();

        Vector const& origin = m_manipulationTransform.GetTranslation();
        Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );
        LineSegment const mouseWorldRay = viewport.ScreenSpaceToWorldSpace( mousePos );
        Vector projectedPoint;

        if ( io.MouseClicked[0] )
        {
            if ( m_isOriginHovered )
            {
                m_mode = ManipulationMode::ScaleXYZ;
            }
            else if ( m_isAxisHoveredX )
            {
                Plane const viewPlane( origin, viewport.GetViewVolume().GetForwardVector() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisX() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::ScaleX;
            }
            else if ( m_isAxisHoveredY )
            {
                Plane const viewPlane( origin, viewport.GetViewVolume().GetForwardVector() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisY() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::ScaleY;
            }
            else if ( m_isAxisHoveredZ )
            {
                Plane const viewPlane( origin, viewport.GetViewVolume().GetForwardVector() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisZ() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::ScaleZ;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_positionOffset = Vector::Zero;
            m_mode = ManipulationMode::None;
        }
    }

    void ScalingGizmo::PerformManipulation( Render::Viewport const& viewport )
    {
        Vector const& originWS = m_manipulationTransform.GetTranslation();

        ImGuiIO& io = ImGui::GetIO();
        if ( io.MouseDownDuration[0] > 0 )
        {
            Vector const mouseDelta( io.MouseDelta );
            Vector const originSS = viewport.WorldSpaceToScreenSpace( originWS );

            if ( !mouseDelta.IsNearZero2() )
            {
                Vector scaleDelta = Vector::Zero;
                Vector manipulationAxis;

                if ( m_mode == ManipulationMode::ScaleXYZ )
                {
                    auto const axisSS = viewport.WorldSpaceToScreenSpace( originWS + m_manipulationTransform.GetUnitAxisZ() );
                    Vector const unitsPerPixel = Vector::One / Vector( axisSS - originSS ).Length2();
                    scaleDelta = Vector::One * -mouseDelta.m_y * unitsPerPixel;
                }

                else if ( m_mode == ManipulationMode::ScaleX || m_mode == ManipulationMode::ScaleY || m_mode == ManipulationMode::ScaleZ )
                {
                    Vector scaleDeltaAxis;

                    if ( m_mode == ManipulationMode::ScaleX )
                    {
                        manipulationAxis = m_isAxisNegatedX ? -m_manipulationTransform.GetUnitAxisX() : m_manipulationTransform.GetUnitAxisX();
                        scaleDeltaAxis = Vector::UnitX;
                    }
                    else if ( m_mode == ManipulationMode::ScaleY )
                    {
                        manipulationAxis = m_isAxisNegatedY ? -m_manipulationTransform.GetUnitAxisY() : m_manipulationTransform.GetUnitAxisY();
                        scaleDeltaAxis = Vector::UnitY;
                    }
                    else if ( m_mode == ManipulationMode::ScaleZ )
                    {
                        manipulationAxis = m_isAxisNegatedZ ? -m_manipulationTransform.GetUnitAxisZ() : m_manipulationTransform.GetUnitAxisZ();
                        scaleDeltaAxis = Vector::UnitZ;
                    }

                    // If we are in world we need to calculate the scale delta axis
                    if ( m_coordinateSpace == CoordinateSpace::World )
                    {
                        Vector const localAxisX = m_targetTransform.GetUnitAxisX();
                        Vector const localAxisY = m_targetTransform.GetUnitAxisY();
                        Vector const localAxisZ = m_targetTransform.GetUnitAxisZ();

                        float const projectedLengthX = Math::Abs( manipulationAxis.GetScalarProjection( localAxisX ) );
                        float const projectedLengthY = Math::Abs( manipulationAxis.GetScalarProjection( localAxisY ) );
                        float const projectedLengthZ = Math::Abs( manipulationAxis.GetScalarProjection( localAxisZ ) );

                        scaleDeltaAxis = Vector( projectedLengthX, projectedLengthY, projectedLengthZ );
                    }

                    //-------------------------------------------------------------------------

                    auto const axisSS = viewport.WorldSpaceToScreenSpace( originWS + manipulationAxis );
                    Vector const unitsPerPixel = Vector::One / Vector( axisSS - originSS ).Length2();

                    LineSegment const manipulationAxisLine( originSS, axisSS );
                    Vector const projectedPointOnAxis = manipulationAxisLine.GetClosestPointOnLine( originSS + mouseDelta );
                    Vector const axisDelta = projectedPointOnAxis - originSS;
                    Vector const axisDeltaDir = Vector::Dot2( axisDelta.GetNormalized2(), manipulationAxisLine.GetDirection() ).m_x > 0 ? Vector::One : Vector::NegativeOne;
                    Vector const axisDeltaLength = axisDelta.Length2();

                    scaleDelta = scaleDeltaAxis * axisDeltaDir * axisDeltaLength * unitsPerPixel;
                }

                //-------------------------------------------------------------------------

                scaleDelta += Vector::One; // Ensure that we maintain current scale for 0 deltas
                Matrix const scalingMatrix = Matrix::FromScale( scaleDelta );
                m_targetTransform = scalingMatrix * m_targetTransform;
            }
        }
    }

    //-------------------------------------------------------------------------

    void ScalingGizmo::InternalUpdate( Render::Viewport const& viewport )
    {
        auto pDrawList = ImGui::GetWindowDrawList();

        //-------------------------------------------------------------------------
        // Draw Mode guides and origin
        //-------------------------------------------------------------------------

        auto drawAxisGuide = [&pDrawList, this] ( Vector const& axis, Color color )
        {
            auto lineLength = ( axis * 10000 );
            auto lineStart = m_origin_SS + lineLength;
            auto lineEnd = m_origin_SS - lineLength;
            pDrawList->AddLine( lineStart.ToFloat2(), lineEnd.ToFloat2(), ConvertColor( color ), 2.0f );
        };

        if ( m_mode == ManipulationMode::ScaleX )
        {
            drawAxisGuide( m_axisDir_SS_X, Colors::Red );
        }
        else if ( m_mode == ManipulationMode::ScaleY )
        {
            drawAxisGuide( m_axisDir_SS_Y, Colors::Lime );
        }
        else if ( m_mode == ManipulationMode::ScaleZ )
        {
            drawAxisGuide( m_axisDir_SS_Z, Colors::Blue );
        }

        m_isOriginHovered = DrawOrigin( m_origin_SS );

        //-------------------------------------------------------------------------
        // Draw axes
        //-------------------------------------------------------------------------

        // If any axes face away from the camera negate them before drawing
        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();
        m_isAxisNegatedX = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_X ).ToFloat() > 0.0f;
        m_isAxisNegatedY = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_Y ).ToFloat() > 0.0f;
        m_isAxisNegatedZ = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_Z ).ToFloat() > 0.0f;

        Vector const drawAxisDir_SS_X = m_isAxisNegatedX ? -m_axisDir_SS_X : m_axisDir_SS_X;
        Vector const drawAxisDir_SS_Y = m_isAxisNegatedY ? -m_axisDir_SS_Y : m_axisDir_SS_Y;
        Vector const drawAxisDir_SS_Z = m_isAxisNegatedZ ? -m_axisDir_SS_Z : m_axisDir_SS_Z;

        // Ensure that gizmo is alway uniform size
        Vector const axisEndPoint_SS_X = m_origin_SS + drawAxisDir_SS_X * g_axisLength;
        Vector const axisEndPoint_SS_Y = m_origin_SS + drawAxisDir_SS_Y * g_axisLength;
        Vector const axisEndPoint_SS_Z = m_origin_SS + drawAxisDir_SS_Z * g_axisLength;

        // Ensure each axis is at least 10 degrees offset from the view direction
        bool const shouldDrawAxis_X = m_offsetBetweenViewFwdAndAxis_WS_X > Degrees( 10.0f );
        bool const shouldDrawAxis_Y = m_offsetBetweenViewFwdAndAxis_WS_Y > Degrees( 10.0f );
        bool const shouldDrawAxis_Z = m_offsetBetweenViewFwdAndAxis_WS_Z > Degrees( 10.0f );

        Color elementColor;

        if ( shouldDrawAxis_X )
        {
            elementColor = ( m_mode == ManipulationMode::ScaleX || m_isAxisHoveredX ) ? g_selectedColor : Colors::Red;
            m_isAxisHoveredX = DrawScaleAxis( m_origin_SS, axisEndPoint_SS_X, elementColor );
        }

        if ( shouldDrawAxis_Y )
        {
            elementColor = ( m_mode == ManipulationMode::ScaleY || m_isAxisHoveredY ) ? g_selectedColor : Colors::Lime;
            m_isAxisHoveredY = DrawScaleAxis( m_origin_SS, axisEndPoint_SS_Y, elementColor );
        }

        if ( shouldDrawAxis_Z )
        {
            elementColor = ( m_mode == ManipulationMode::ScaleZ || m_isAxisHoveredZ ) ? g_selectedColor : Colors::Blue;
            m_isAxisHoveredZ = DrawScaleAxis( m_origin_SS, axisEndPoint_SS_Z, elementColor );
        }

        //-------------------------------------------------------------------------
        // Switch Mode and calculate the translation delta
        //-------------------------------------------------------------------------

        UpdateMode( viewport );
        PerformManipulation( viewport );
    }
}
#endif