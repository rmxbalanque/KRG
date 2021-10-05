#include "TranslationGizmo.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    static float const g_axisLength = 75.0f;
    static float const g_axisThickness = 5.0f;
    static float const g_axisGuideThickness = 3.0f;
    static float const g_originSphereRadius = 5.0f;
    static float const g_originSphereAxisOffset = g_originSphereRadius + 4.0f;
    static float const g_planeWidgetLength = 20.0f;
    static float const g_planeWidgetOffset = 15.0f;
    static Color const g_selectedColor = Colors::Yellow;
    static Color const g_planeColor = Colors::Gray;

    //-------------------------------------------------------------------------

    // Returns true is mouse is hovering over the axis
    bool TranslationGizmo::DrawTranslationAxis( Vector const& start, Vector const& end, Color color )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        Vector const arrowDirection = ( end - start ).GetNormalized2();
        if ( arrowDirection.IsNearZero4() )
        {
            return false;
        }

        float const arrowHeadThickness = g_axisThickness * 2;
        float const arrowHeadLength = g_axisThickness * 2;

        Float2 const arrowHeadBase = end - ( arrowDirection * arrowHeadLength );
        Float2 const orthogonalDir( arrowDirection.m_y, -arrowDirection.m_x );
        Float2 const orthogonalOffset = orthogonalDir * arrowHeadThickness;
        Float2 const orthogonalBoundsOffset = orthogonalDir * ( arrowHeadThickness * 0.75f );

        //-------------------------------------------------------------------------

        Vector const offsetStart = start + ( arrowDirection * g_originSphereAxisOffset );
        uint32 const axisColor = ConvertColor( color );
        pDrawList->AddLine( offsetStart.ToFloat2(), arrowHeadBase + arrowDirection, axisColor, g_axisThickness );
        pDrawList->AddTriangleFilled( end.ToFloat2(), arrowHeadBase - orthogonalOffset, arrowHeadBase + orthogonalOffset, axisColor );

        //-------------------------------------------------------------------------

        LineSegment const arrowLine( offsetStart, end );
        Vector const mousePos = io.MousePos;
        float const mouseToLineDistance = arrowLine.GetDistanceBetweenSegmentAndPoint( mousePos );
        bool const isHovered = mouseToLineDistance < ( ( g_axisThickness / 2 ) + 3.0f );

        return isHovered;
    }

    // Returns true is mouse is hovering over the quad
    bool TranslationGizmo::DrawPlaneTranslationWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        Vector const planeAxisDeltaNearX = axis0 * g_planeWidgetOffset;
        Vector const planeAxisDeltaFarX = axis0 * ( g_planeWidgetLength + g_planeWidgetOffset );
        Vector const planeAxisDeltaNearY = axis1 * g_planeWidgetOffset;
        Vector const planeAxisDeltaFarY = axis1 * ( g_planeWidgetLength + g_planeWidgetOffset );

        Vector quadVerts[4] =
        {
            ( origin + planeAxisDeltaFarX + planeAxisDeltaNearY ),
            ( origin + planeAxisDeltaNearX + planeAxisDeltaNearY ),
            ( origin + planeAxisDeltaNearX + planeAxisDeltaFarY ),
            ( origin + planeAxisDeltaFarX + planeAxisDeltaFarY ),
        };

        float const distanceAcross0 = quadVerts[0].GetDistance2( quadVerts[2] );
        float const distanceAcross1 = quadVerts[1].GetDistance2( quadVerts[3] );
        if ( distanceAcross0 < 5.0f || distanceAcross1 < 5.0f )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        ImVec2 drawQuadVerts[4] =
        {
            quadVerts[0].ToFloat2(),
            quadVerts[1].ToFloat2(),
            quadVerts[2].ToFloat2(),
            quadVerts[3].ToFloat2(),
        };

        pDrawList->AddPolyline( drawQuadVerts, 4, ConvertColor( color ), true, 1.0f );
        pDrawList->AddConvexPolyFilled( drawQuadVerts, 4, ConvertColor( color.GetAlphaVersion( 0.75f ) ) );

        //-------------------------------------------------------------------------

        Vector const mousePos = io.MousePos;
        auto LeftHandTest = [&mousePos, &drawQuadVerts] ( int32 side0, int32 side1 )
        {
            float D = ( drawQuadVerts[side1].x - drawQuadVerts[side0].x ) * ( mousePos.m_y - drawQuadVerts[side0].y ) - ( mousePos.m_x - drawQuadVerts[side0].x ) * ( drawQuadVerts[side1].y - drawQuadVerts[side0].y );
            return D > 0;
        };

        bool const allInsideClockwise = LeftHandTest( 0, 1 ) && LeftHandTest( 1, 2 ) && LeftHandTest( 2, 3 ) && LeftHandTest( 3, 0 );
        bool const allInsideCounterClockwise = LeftHandTest( 0, 3 ) && LeftHandTest( 3, 2 ) && LeftHandTest( 2, 1 ) && LeftHandTest( 1, 0 );
        bool const isHovered = allInsideClockwise || allInsideCounterClockwise;
        return isHovered;
    }

    //-------------------------------------------------------------------------

    void TranslationGizmo::UpdateMode( Render::Viewport const& viewport )
    {
        ImGuiIO& io = ImGui::GetIO();

        Vector const& origin = m_manipulationTransform.GetTranslation();
        Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );
        LineSegment const mouseWorldRay = viewport.ScreenSpaceToWorldSpace( mousePos );
        Vector projectedPoint;

        if ( io.MouseClicked[0] )
        {
            if ( m_isAxisHoveredX )
            {
                Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisX() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::TranslateX;
            }
            else if ( m_isAxisHoveredY )
            {
                Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisY() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::TranslateY;
            }
            else if ( m_isAxisHoveredZ )
            {
                Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisZ() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_mode = ManipulationMode::TranslateZ;
            }
            else if ( m_isPlaneHoveredXY )
            {
                Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisZ() );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_mode = ManipulationMode::TranslateXY;
            }
            else if ( m_isPlaneHoveredXZ )
            {
                Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisY() );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_mode = ManipulationMode::TranslateXZ;
            }
            else if ( m_isPlaneHoveredYZ )
            {
                Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisX() );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_mode = ManipulationMode::TranslateYZ;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_translationOffset = Vector::Zero;
            m_mode = ManipulationMode::None;
        }
    }

    void TranslationGizmo::PerformManipulation( Render::Viewport const& viewport )
    {
        Vector const& origin = m_manipulationTransform.GetTranslation();

        ImGuiIO& io = ImGui::GetIO();
        if ( io.MouseDownDuration[0] > 0 )
        {
            Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );
            LineSegment const mouseWorldRay = viewport.ScreenSpaceToWorldSpace( mousePos );
            Vector projectedPoint;
            Vector translationDelta = Vector::Zero;

            Vector const mouseDelta( io.MouseDelta );
            if ( !mouseDelta.IsNearZero2() )
            {
                if ( m_mode == ManipulationMode::TranslateX )
                {
                    Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisX() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_mode == ManipulationMode::TranslateY )
                {
                    Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisY() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_mode == ManipulationMode::TranslateZ )
                {
                    Plane const viewPlane( origin, viewport.GetViewForwardDirection() );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetUnitAxisZ() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_mode == ManipulationMode::TranslateXY )
                {
                    Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisZ() );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }

                else if ( m_mode == ManipulationMode::TranslateXZ )
                {
                    Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisY() );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }

                else if ( m_mode == ManipulationMode::TranslateYZ )
                {
                    Plane translationPlane( origin, m_manipulationTransform.GetUnitAxisX() );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }
            }

            m_targetTransform.SetTranslation( origin + translationDelta );
        }
    }

    //-------------------------------------------------------------------------

    void TranslationGizmo::InternalUpdate( Render::Viewport const& viewport )
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
            pDrawList->AddLine( lineStart.ToFloat2(), lineEnd.ToFloat2(), ConvertColor( color ), g_axisGuideThickness );
        };

        if ( m_mode == ManipulationMode::TranslateX )
        {
            drawAxisGuide( m_axisDir_SS_X, Colors::Red );
        }
        else if ( m_mode == ManipulationMode::TranslateY )
        {
            drawAxisGuide( m_axisDir_SS_Y, Colors::Lime );
        }
        else if ( m_mode == ManipulationMode::TranslateZ )
        {
            drawAxisGuide( m_axisDir_SS_Z, Colors::Blue );
        }

        pDrawList->AddCircleFilled( m_origin_SS.ToFloat2(), g_originSphereRadius, Colors::White );

        //-------------------------------------------------------------------------
        // Draw axes
        //-------------------------------------------------------------------------

        // If any axes face away from the camera negate them before drawing
        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();
        bool const negateAxisX = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_X ).ToFloat() > 0.0f;
        bool const negateAxisY = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_Y ).ToFloat() > 0.0f;
        bool const negateAxisZ = Vector::Dot3( viewForwardDir_WS, m_axisDir_WS_Z ).ToFloat() > 0.0f;

        Vector const drawAxisDir_SS_X = negateAxisX ? -m_axisDir_SS_X : m_axisDir_SS_X;
        Vector const drawAxisDir_SS_Y = negateAxisY ? -m_axisDir_SS_Y : m_axisDir_SS_Y;
        Vector const drawAxisDir_SS_Z = negateAxisZ ? -m_axisDir_SS_Z : m_axisDir_SS_Z;

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
            elementColor = ( m_mode == ManipulationMode::TranslateX || m_mode == ManipulationMode::TranslateXY || m_mode == ManipulationMode::TranslateXZ || m_isAxisHoveredX ) ? g_selectedColor : Colors::Red;
            m_isAxisHoveredX = DrawTranslationAxis( m_origin_SS, axisEndPoint_SS_X, elementColor );
        }

        if ( shouldDrawAxis_Y )
        {
            elementColor = ( m_mode == ManipulationMode::TranslateY || m_mode == ManipulationMode::TranslateXY || m_mode == ManipulationMode::TranslateYZ || m_isAxisHoveredY ) ? g_selectedColor : Colors::Green;
            m_isAxisHoveredY = DrawTranslationAxis( m_origin_SS, axisEndPoint_SS_Y, elementColor );
        }

        if ( shouldDrawAxis_Z )
        {
            elementColor = ( m_mode == ManipulationMode::TranslateZ || m_mode == ManipulationMode::TranslateXZ || m_mode == ManipulationMode::TranslateYZ || m_isAxisHoveredZ ) ? g_selectedColor : Colors::Blue;
            m_isAxisHoveredZ = DrawTranslationAxis( m_origin_SS, axisEndPoint_SS_Z, elementColor );
        }

        //-------------------------------------------------------------------------
        // Draw planes
        //-------------------------------------------------------------------------

        elementColor = ( m_isPlaneHoveredXY || m_mode == ManipulationMode::TranslateXY ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredXY = DrawPlaneTranslationWidget( m_origin_SS, drawAxisDir_SS_X, drawAxisDir_SS_Y, elementColor );

        elementColor = ( m_isPlaneHoveredXZ || m_mode == ManipulationMode::TranslateXZ ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredXZ = DrawPlaneTranslationWidget( m_origin_SS, drawAxisDir_SS_X, drawAxisDir_SS_Z, elementColor );

        elementColor = ( m_isPlaneHoveredYZ || m_mode == ManipulationMode::TranslateYZ ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredYZ = DrawPlaneTranslationWidget( m_origin_SS, drawAxisDir_SS_Y, drawAxisDir_SS_Z, elementColor );

        //-------------------------------------------------------------------------
        // Switch Mode and calculate the translation delta
        //-------------------------------------------------------------------------

        UpdateMode( viewport );
        PerformManipulation( viewport );
    }
}
#endif