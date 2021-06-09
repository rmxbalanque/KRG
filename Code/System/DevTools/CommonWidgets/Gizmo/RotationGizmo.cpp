#include "RotationGizmo.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    static uint32 const g_halfCircleSegmentCount = 64;
    static float const g_widgetRadius = 40.0f;
    static float const g_gizmoPickingSelectionBuffer = 3.0f;
    static Color const g_selectedColor = Colors::Yellow;

    //-------------------------------------------------------------------------

    bool RotationGizmo::DrawScreenRotationGizmo( Math::Viewport const& viewport )
    {
        auto pDrawList = ImGui::GetWindowDrawList();

        Color const innerColor = ( m_isScreenRotationWidgetHovered ) ? g_selectedColor : Colors::White;
        Color const outerColor = ( m_isScreenRotationWidgetHovered ) ? g_selectedColor : Colors::White.GetAlphaVersion( 0.5f );
        uint32 const imguiInnerColor = ConvertColor( innerColor );
        uint32 const imguiOuterColor = ConvertColor( outerColor );

        // Draw current Rotation Covered
        //-------------------------------------------------------------------------

        pDrawList->AddCircleFilled( m_origin_SS.ToFloat2(), 3.0f, imguiInnerColor, 20 );
        pDrawList->AddCircle( m_origin_SS.ToFloat2(), 8.0f, imguiInnerColor, 20, 2.0f );
        pDrawList->AddCircle( m_origin_SS.ToFloat2(), g_widgetRadius, imguiOuterColor, 50, 3.0f );

        // Hover test
        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        Vector const mousePos = io.MousePos;
        float const distance = mousePos.GetDistance2( m_origin_SS );
        bool const isHovered = ( distance >= ( g_widgetRadius - g_gizmoPickingSelectionBuffer ) && distance <= ( g_widgetRadius + g_gizmoPickingSelectionBuffer ) );
        return isHovered;
    }

    bool RotationGizmo::DrawRotationWidget( Math::Viewport const& viewport, Vector const& axisOfRotation_WS, Color color )
    {
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        //-------------------------------------------------------------------------

        // Calculate rough scaling factor
        Vector const viewRightDir_WS = viewport.GetViewRightDirection();
        Float2 const offsetPosition = viewport.WorldSpaceToScreenSpace( m_origin_WS + viewRightDir_WS );
        float const pixelsPerM = 1.0f / m_origin_SS.GetDistance2( offsetPosition );
        float const scaleMultiplier = g_widgetRadius * pixelsPerM;

        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        Vector const mousePos = io.MousePos;

        bool isHovered = false;
        int32 const numPoints = g_halfCircleSegmentCount;
        ImVec2 circlePointsSS[numPoints];

        Vector const startRotationVector = axisOfRotation_WS.Cross3( viewport.GetViewForwardDirection().GetNegated() ).GetNormalized3().GetNegated();
        Radians const angleStepDelta = Radians::Pi / (float) ( numPoints - 1 );
        for ( auto i = 0; i < numPoints; i++ )
        {
            Quaternion deltaRot = Quaternion( axisOfRotation_WS, angleStepDelta * i );
            Vector pointOnCircle_WS = m_origin_WS + ( ( deltaRot.RotateVector( startRotationVector ).GetNormalized3() ) * scaleMultiplier );
            Float2 pointOnCircle_SS = viewport.WorldSpaceToScreenSpace( pointOnCircle_WS );
            circlePointsSS[i] = pointOnCircle_SS;

            if ( !isHovered )
            {
                float distance = mousePos.GetDistance2( pointOnCircle_SS );
                if ( distance < g_gizmoPickingSelectionBuffer )
                {
                    isHovered = true;
                }
            }
        }

        // Disable the manipulation widget when it is within a deadzone
        Vector const widgetStartPoint = Vector( circlePointsSS[0] );
        Vector const widgetMidPoint = Vector( circlePointsSS[numPoints / 2] );
        Vector const widgetEndPoint = Vector( circlePointsSS[numPoints - 1] );

        Radians const angleBetweenMidAndEnd = ( widgetEndPoint - widgetStartPoint ).GetAngleBetween( widgetMidPoint - widgetStartPoint );
        bool const isDisabled = angleBetweenMidAndEnd < Degrees( 10.0f );
        if ( isDisabled )
        {
            color = color.GetAlphaVersion( 0.45f );
            isHovered = false;
        }

        pDrawList->AddPolyline( circlePointsSS, g_halfCircleSegmentCount, ConvertColor( color ), false, 3.0f );
        return isHovered;
    }

    void RotationGizmo::DrawManipulationWidget( Math::Viewport const& viewport, Vector const& axisOfRotation_ws, Vector const& axisOfRotation_ss, Color color )
    {
        static float const gizmoRadius = 40.0f;
        static float const gizmoThickness = 3.0f;

        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        Vector const mousePos = io.MousePos;

        //-------------------------------------------------------------------------

        auto axisOfRotationColor = ConvertColor( color );
        auto axisOfRotationColorAlpha = ConvertColor( color.GetAlphaVersion( 0.75f ) );

        auto lineLength = ( axisOfRotation_ss * 10000 );
        auto lineStart = m_origin_SS + lineLength;
        auto lineEnd = m_origin_SS - lineLength;
        pDrawList->AddLine( lineStart.ToFloat2(), lineEnd.ToFloat2(), axisOfRotationColorAlpha, 1.0f );

        //-------------------------------------------------------------------------

        // Calculate rough scaling factor
        Float2 const offsetPosition = viewport.WorldSpaceToScreenSpace( m_origin_WS + viewport.GetViewRightDirection() );
        float const pixelsPerM = 1.0f / m_origin_SS.GetDistance2( offsetPosition );
        float const scaleMultiplier = gizmoRadius * pixelsPerM;

        // Draw manipulation circle
        //-------------------------------------------------------------------------

        // Find the start point on the rotation plane
        LineSegment mouseRay = viewport.ScreenSpaceToWorldSpace( m_rotationStartMousePosition );
        Plane planeOfRotation( m_origin_WS, axisOfRotation_ws );
        Vector intersectionPoint;
        bool const intersectionResult = planeOfRotation.IntersectLine( mouseRay, intersectionPoint );

        //-------------------------------------------------------------------------

        Vector const startRotationVector = ( intersectionPoint - m_origin_WS ).GetNormalized3();

        ImVec2 outerCirclePointsSS[g_halfCircleSegmentCount * 2];
        Radians angleStepDelta = Radians::TwoPi / (float) ( g_halfCircleSegmentCount * 2 - 1 );
        for ( auto i = 0; i < g_halfCircleSegmentCount * 2; i++ )
        {
            Quaternion deltaRot = Quaternion( axisOfRotation_ws, angleStepDelta * i );
            Vector pointOnCircle_WS = m_origin_WS + ( ( deltaRot.RotateVector( startRotationVector ).GetNormalized3() ) * scaleMultiplier );
            Float2 pointOnCircle_SS = viewport.WorldSpaceToScreenSpace( pointOnCircle_WS );
            outerCirclePointsSS[i] = pointOnCircle_SS;
        }

        pDrawList->AddPolyline( outerCirclePointsSS, g_halfCircleSegmentCount * 2, axisOfRotationColor, false, gizmoThickness );

        //-------------------------------------------------------------------------

        if ( Math::Abs( m_rotationDeltaAngle ) < Degrees( 3.0f ) )
        {
            Vector const pointOnCircle_WS = m_origin_WS + ( startRotationVector.GetNormalized3() * scaleMultiplier );
            Float2 const pointOnCircle_SS = viewport.WorldSpaceToScreenSpace( pointOnCircle_WS );

            pDrawList->AddCircleFilled( m_origin_SS, 3.0f, ConvertColor( Colors::White ) );
            pDrawList->AddLine( m_origin_SS, pointOnCircle_SS, ConvertColor( Colors::White ), 1.0f );
            pDrawList->AddCircleFilled( pointOnCircle_SS, 3.0f, ConvertColor( Colors::Orange ) );
        }
        else
        {
            uint32 const numCoveredCirclePoints = (uint32) Math::Ceiling( Math::Abs( m_rotationDeltaAngle.ToFloat() / Math::TwoPi ) * ( g_halfCircleSegmentCount * 2 ) );
            ImVec2 innerCirclePointsSS[g_halfCircleSegmentCount * 2 + 2];
            angleStepDelta = m_rotationDeltaAngle / (float) numCoveredCirclePoints;
            for ( auto i = 0u; i < numCoveredCirclePoints; i++ )
            {
                Quaternion deltaRot = Quaternion( axisOfRotation_ws, angleStepDelta * (float) i );
                Vector pointOnCircle_WS = m_origin_WS + ( ( deltaRot.RotateVector( startRotationVector ).GetNormalized3() ) * scaleMultiplier );
                Float2 pointOnCircle_SS = viewport.WorldSpaceToScreenSpace( pointOnCircle_WS );
                innerCirclePointsSS[i] = pointOnCircle_SS;
            }

            innerCirclePointsSS[numCoveredCirclePoints] = m_origin_SS;

            pDrawList->AddConvexPolyFilled( innerCirclePointsSS, numCoveredCirclePoints + 1, ConvertColor( Colors::Orange.GetAlphaVersion( 0.5f ) ) );
            pDrawList->AddPolyline( innerCirclePointsSS, numCoveredCirclePoints, ConvertColor( Colors::Orange ), false, 4.0f );
            pDrawList->AddLine( m_origin_SS, innerCirclePointsSS[numCoveredCirclePoints - 1], ConvertColor( Colors::Orange ), 1.0f );
            pDrawList->AddLine( m_origin_SS, innerCirclePointsSS[0], ConvertColor( Colors::Orange ), 1.0f );
            pDrawList->AddCircleFilled( m_origin_SS, 3.0f, ConvertColor( Colors::White ) );

            // Draw rotation text
            //-------------------------------------------------------------------------

            static char buff[16];
            Printf( buff, 255, " %.2f", (float) m_rotationDeltaAngle.ToDegrees() );

            auto textSize = ImGui::CalcTextSize( buff );
            Vector const textPosition = Vector( innerCirclePointsSS[numCoveredCirclePoints - 1] ) - Float2( textSize.x / 2, 0 );
            pDrawList->AddRectFilled( textPosition, textPosition + Float2( textSize.x + 3, textSize.y ), ConvertColor( Colors::Black.GetAlphaVersion( 0.5f ) ), 3.0f );
            pDrawList->AddText( textPosition, ConvertColor( Colors::White ), buff );
        }
    }

    void RotationGizmo::UpdateMode( Math::Viewport const& viewport )
    {
        ImGuiIO& io = ImGui::GetIO();

        if ( io.MouseClicked[1] )
        {
            m_mode = ManipulationMode::None;
            return;
        }

        //-------------------------------------------------------------------------

        if ( io.MouseClicked[0] )
        {
            m_rotationStartMousePosition = io.MousePos;

            if ( m_isAxisRotationWidgetHoveredX )
            {
                m_originalStartRotation = m_targetTransform.GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitX : m_manipulationTransform.GetUnitAxisX();
                m_mode = ManipulationMode::RotateX;
            }
            else if ( m_isAxisRotationWidgetHoveredY )
            {
                m_originalStartRotation = m_targetTransform.GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitY : m_manipulationTransform.GetUnitAxisY();
                m_mode = ManipulationMode::RotateY;
            }
            else if ( m_isAxisRotationWidgetHoveredZ )
            {
                m_originalStartRotation = m_targetTransform.GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitZ : m_manipulationTransform.GetUnitAxisZ();
                m_mode = ManipulationMode::RotateZ;
            }
            // Use the world space transform for screen space rotations even if we are in object space
            else if ( m_isScreenRotationWidgetHovered )
            {
                m_originalStartRotation = m_targetTransform.GetRotation();
                m_rotationAxis = viewport.GetViewForwardDirection();
                m_mode = ManipulationMode::RotateScreen;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_rotationStartMousePosition = Float2::Zero;
            m_mode = ManipulationMode::None;
        }
    }

    void RotationGizmo::PerformManipulation( Math::Viewport const& viewport )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();

        if ( m_mode == ManipulationMode::None )
        {
            return;
        }

        if ( io.MouseDownDuration[0] > 0 )
        {
            Vector const mouseDelta( io.MouseDelta );
            Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );

            if ( m_mode == ManipulationMode::RotateScreen )
            {
                // Calculate rotation angle
                Vector const originalScreenDir = Vector( m_rotationStartMousePosition - m_origin_SS ).GetNormalized2();
                Vector const desiredRotationScreenDir = ( mousePos - m_origin_SS ).GetNormalized2();
                m_rotationDeltaAngle = originalScreenDir.GetAngleBetween( desiredRotationScreenDir );

                // Adjust direction of rotation
                bool isRotationDirectionPositive = Vector::Cross3( originalScreenDir, desiredRotationScreenDir ).m_z > 0.0f;
                if ( !isRotationDirectionPositive )
                {
                    m_rotationDeltaAngle = -m_rotationDeltaAngle;
                }
            }
            else
            {
                Plane rotationPlane( m_origin_WS, m_rotationAxis );

                LineSegment const startMouseRay = viewport.ScreenSpaceToWorldSpace( m_rotationStartMousePosition );
                LineSegment const newMouseRay = viewport.ScreenSpaceToWorldSpace( mousePos );

                Vector startIntersectionPoint;
                rotationPlane.IntersectLine( startMouseRay, startIntersectionPoint );

                Vector newIntersectionPoint;
                rotationPlane.IntersectLine( newMouseRay, newIntersectionPoint );

                //-------------------------------------------------------------------------

                // Calculate rotation angle
                Vector const originalVector_WS = startIntersectionPoint - m_origin_WS;
                Vector const newVector_WS = newIntersectionPoint - m_origin_WS;

                m_rotationDeltaAngle = originalVector_WS.GetAngleBetween( newVector_WS );
                bool isRotationDirectionPositive = Vector::Dot3( Vector::Cross3( originalVector_WS, newVector_WS ).GetNormalized3(), m_rotationAxis ).ToFloat() > 0.0f;
                if ( !isRotationDirectionPositive )
                {
                    m_rotationDeltaAngle = -m_rotationDeltaAngle;
                }

            }

            //-------------------------------------------------------------------------

            if ( !Math::IsNearZero( m_rotationDeltaAngle, Degrees( 1.0f ).ToRadians() ) )
            {
                // Calculate rotation delta and apply it
                Quaternion const rotationDelta( m_rotationAxis, m_rotationDeltaAngle );
                m_targetTransform.SetRotationMaintainingScale( m_originalStartRotation * rotationDelta );
            }
            else
            {
                m_rotationDeltaAngle = 0.0f;
            }
        }
    }

    //-------------------------------------------------------------------------

    void RotationGizmo::InternalUpdate( Math::Viewport const& viewport )
    {
        auto pDrawList = ImGui::GetWindowDrawList();

        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();

        //-------------------------------------------------------------------------

        if ( m_mode == ManipulationMode::None )
        {
            m_isScreenRotationWidgetHovered = DrawScreenRotationGizmo( viewport );

            Color elementColor = m_isAxisRotationWidgetHoveredX ? g_selectedColor : Colors::Red;
            m_isAxisRotationWidgetHoveredX = DrawRotationWidget( viewport, m_axisDir_WS_X, elementColor );

            elementColor = m_isAxisRotationWidgetHoveredY ? g_selectedColor : Colors::Lime;
            m_isAxisRotationWidgetHoveredY = DrawRotationWidget( viewport, m_axisDir_WS_Y, elementColor );

            elementColor = m_isAxisRotationWidgetHoveredZ ? g_selectedColor : Colors::Blue;
            m_isAxisRotationWidgetHoveredZ = DrawRotationWidget( viewport, m_axisDir_WS_Z, elementColor );

            // Individual axes take precedence over screen rotation
            if ( m_isScreenRotationWidgetHovered )
            {
                m_isScreenRotationWidgetHovered &= !( m_isAxisRotationWidgetHoveredX || m_isAxisRotationWidgetHoveredY || m_isAxisRotationWidgetHoveredZ );
            }

            // Z takes precedence over X & Y, Y takes precedence over X
            if ( m_isAxisRotationWidgetHoveredZ )
            {
                m_isAxisRotationWidgetHoveredX = m_isAxisRotationWidgetHoveredY = false;
            }

            if ( m_isAxisRotationWidgetHoveredY )
            {
                m_isAxisRotationWidgetHoveredX = false;
            }
        }
        else
        {
            if ( m_mode == ManipulationMode::RotateX )
            {
                DrawManipulationWidget( viewport, m_axisDir_WS_X, m_axisDir_SS_X, Colors::Red );
            }
            else if ( m_mode == ManipulationMode::RotateY )
            {
                DrawManipulationWidget( viewport, m_axisDir_WS_Y, m_axisDir_SS_Y, Colors::Lime );
            }
            else if ( m_mode == ManipulationMode::RotateZ )
            {
                DrawManipulationWidget( viewport, m_axisDir_WS_Z, m_axisDir_SS_Z, Colors::Blue );
            }
            else if ( m_mode == ManipulationMode::RotateScreen )
            {
                DrawManipulationWidget( viewport, viewport.GetViewForwardDirection(), Vector::UnitZ, Colors::White );
            }
        }

        UpdateMode( viewport );
        PerformManipulation( viewport );
    }
}