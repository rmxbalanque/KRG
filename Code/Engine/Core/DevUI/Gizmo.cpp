#include "Gizmo.h"
#include "imgui.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    // Rotation
    constexpr static uint32 const       g_halfCircleSegmentCount = 64;
    constexpr static float const        g_widgetRadius = 40.0f;
    constexpr static float const        g_gizmoPickingSelectionBuffer = 3.0f;
    static Color const                  g_selectedColor = Colors::Yellow;

    // Scale
    constexpr static float const        g_axisLength = 75.0f;
    constexpr static float const        g_axisThickness = 5.0f;
    constexpr static float const        g_originSphereRadius = 4.0f;
    constexpr static float const        g_selectedOriginSphereRadius = 12.0f;
    constexpr static float const        g_originSphereAxisOffset = g_originSphereRadius + 8.0f;
    static Color const                  g_planeColor = Colors::Gray;

    // Translation
    constexpr static float const        g_axisGuideThickness = 3.0f;
    constexpr static float const        g_planeWidgetLength = 20.0f;
    constexpr static float const        g_planeWidgetOffset = 15.0f;

    //-------------------------------------------------------------------------

    void Gizmo::SwitchToNextMode()
    {
        switch ( m_gizmoMode )
        {
            case GizmoMode::None:
            SwitchMode( GizmoMode::Rotation );
            break;

            case GizmoMode::Rotation:
            SwitchMode( GizmoMode::Translation );
            break;

            case GizmoMode::Translation:
            SwitchMode( GizmoMode::Scale );
            break;

            case GizmoMode::Scale:
            SwitchMode( GizmoMode::Rotation );
            break;
        }
    }

    void Gizmo::SwitchMode( GizmoMode newMode )
    {
        ResetState();
        m_gizmoMode = newMode;
        m_manipulationMode = ManipulationMode::None;
    }

    void Gizmo::SetTargetTransform( Transform* pTargetTransform )
    {
        m_pTargetTransform = pTargetTransform;
        ResetState();
    }

    void Gizmo::ResetState()
    {
        m_isScreenRotationWidgetHovered = false;
        m_isAxisRotationWidgetHoveredX = false;
        m_isAxisRotationWidgetHoveredY = false;
        m_isAxisRotationWidgetHoveredZ = false;

        m_rotationStartMousePosition = Float2::Zero;
        m_originalStartRotation = Quaternion::Identity;
        m_rotationAxis = Vector::Zero;
        m_rotationDeltaAngle = 0.0f;

        m_translationOffset = Vector::Zero;
        m_isAxisHoveredX = false;
        m_isAxisHoveredY = false;
        m_isAxisHoveredZ = false;
        m_isPlaneHoveredXY = false;
        m_isPlaneHoveredXZ = false;
        m_isPlaneHoveredYZ = false;

        m_positionOffset = Vector::Zero;
        m_isOriginHovered = false;
        m_isAxisNegatedX = false;
        m_isAxisNegatedY = false;
        m_isAxisNegatedZ = false;
    }

    bool Gizmo::Draw( Render::Viewport const& originalViewport )
    {
        if ( m_pTargetTransform == nullptr )
        {
            return false;
        }

        Render::Viewport viewport = originalViewport;
        viewport.Resize( Float2( ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin() ), originalViewport.GetDimensions() );

        //-------------------------------------------------------------------------

        m_manipulationTransform = *m_pTargetTransform;
        m_origin_WS = m_manipulationTransform.GetTranslation();

        if ( !viewport.GetViewVolume().Contains( m_origin_WS ) )
        {
            return false;
        }

        if ( m_coordinateSpace == CoordinateSpace::World )
        {
            m_manipulationTransform.SetRotation( Quaternion::Identity );
        }

        // Calculate shared data
        //-------------------------------------------------------------------------

        m_axisDir_WS_X = m_manipulationTransform.GetAxisX();
        m_axisDir_WS_Y = m_manipulationTransform.GetAxisY();
        m_axisDir_WS_Z = m_manipulationTransform.GetAxisZ();

        m_axisEndPoint_WS_X = m_origin_WS + ( m_axisDir_WS_X * 0.01f );
        m_axisEndPoint_WS_Y = m_origin_WS + ( m_axisDir_WS_Y * 0.01f );
        m_axisEndPoint_WS_Z = m_origin_WS + ( m_axisDir_WS_Z * 0.01f );

        m_origin_SS = Vector( viewport.WorldSpaceToScreenSpace( m_origin_WS ) );

        m_axisEndPoint_SS_X = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_X ) );
        m_axisEndPoint_SS_Y = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_Y ) );
        m_axisEndPoint_SS_Z = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_Z ) );

        m_axisDir_SS_X = ( m_axisEndPoint_SS_X - m_origin_SS ).GetNormalized2();
        m_axisDir_SS_Y = ( m_axisEndPoint_SS_Y - m_origin_SS ).GetNormalized2();
        m_axisDir_SS_Z = ( m_axisEndPoint_SS_Z - m_origin_SS ).GetNormalized2();

        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();
        Vector const viewRightDir_WS = viewport.GetViewRightDirection();

        m_offsetBetweenViewFwdAndAxis_WS_X = Math::Min( Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_X ), Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_X.GetNegated() ) );
        m_offsetBetweenViewFwdAndAxis_WS_Y = Math::Min( Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_Y ), Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_Y.GetNegated() ) );
        m_offsetBetweenViewFwdAndAxis_WS_Z = Math::Min( Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_Z ), Math::GetAngleBetweenVectors( viewForwardDir_WS, m_axisDir_WS_Z.GetNegated() ) );

        //-------------------------------------------------------------------------

        Transform const originalTransform = *m_pTargetTransform;

        if ( m_gizmoMode == GizmoMode::Rotation )
        {
            Rotation_Update( viewport );
        }
        else if ( m_gizmoMode == GizmoMode::Translation )
        {
            Translation_Update( viewport );
        }
        else if ( m_gizmoMode == GizmoMode::Scale )
        {
            Scale_Update( viewport );
        }

        return *m_pTargetTransform != originalTransform;
    }

    //-------------------------------------------------------------------------

    bool Gizmo::Rotation_DrawScreenGizmo( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

        auto pDrawList = ImGui::GetWindowDrawList();

        Color const innerColor = ( m_isScreenRotationWidgetHovered ) ? g_selectedColor : Colors::White;
        Color const outerColor = ( m_isScreenRotationWidgetHovered ) ? g_selectedColor : Colors::White.GetAlphaVersion( 0.5f );
        uint32 const imguiInnerColor = ConvertColor( innerColor );
        uint32 const imguiOuterColor = ConvertColor( outerColor );

        // Draw current Rotation Covered
        //-------------------------------------------------------------------------

        pDrawList->AddCircleFilled( m_origin_SS.ToFloat2(), g_originSphereRadius, imguiInnerColor, 20 );
        pDrawList->AddCircle( m_origin_SS.ToFloat2(), g_widgetRadius, imguiOuterColor, 50, 3.0f );

        // Hover test
        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        Vector const mousePos = io.MousePos;
        float const distance = mousePos.GetDistance2( m_origin_SS );
        bool const isHovered = ( distance >= ( g_widgetRadius - g_gizmoPickingSelectionBuffer ) && distance <= ( g_widgetRadius + g_gizmoPickingSelectionBuffer ) );
        return isHovered;
    }

    bool Gizmo::Rotation_DrawWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_WS, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

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

        Radians const angleBetweenMidAndEnd = Math::GetAngleBetweenVectors( ( widgetEndPoint - widgetStartPoint ), ( widgetMidPoint - widgetStartPoint ) );
        bool const isDisabled = angleBetweenMidAndEnd < Degrees( 10.0f );
        if ( isDisabled )
        {
            color = color.GetAlphaVersion( 0.45f );
            isHovered = false;
        }

        pDrawList->AddPolyline( circlePointsSS, g_halfCircleSegmentCount, ConvertColor( color ), false, 3.0f );
        return isHovered;
    }

    void Gizmo::Rotation_DrawManipulationWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_ws, Vector const& axisOfRotation_ss, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

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
        Plane planeOfRotation = Plane::FromNormalAndPoint( axisOfRotation_ws, m_origin_WS );
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

        if ( Math::Abs( (float) m_rotationDeltaAngle ) < (float) Degrees( 3.0f ) )
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

    void Gizmo::Rotation_UpdateMode( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

        ImGuiIO& io = ImGui::GetIO();

        if ( io.MouseClicked[1] )
        {
            m_manipulationMode = ManipulationMode::None;
            return;
        }

        //-------------------------------------------------------------------------

        if ( io.MouseClicked[0] )
        {
            m_rotationStartMousePosition = io.MousePos;

            if ( m_isAxisRotationWidgetHoveredX )
            {
                m_originalStartRotation = m_pTargetTransform->GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitX : m_manipulationTransform.GetAxisX();
                m_manipulationMode = ManipulationMode::RotateX;
            }
            else if ( m_isAxisRotationWidgetHoveredY )
            {
                m_originalStartRotation = m_pTargetTransform->GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitY : m_manipulationTransform.GetAxisY();
                m_manipulationMode = ManipulationMode::RotateY;
            }
            else if ( m_isAxisRotationWidgetHoveredZ )
            {
                m_originalStartRotation = m_pTargetTransform->GetRotation();
                m_rotationAxis = ( m_coordinateSpace == CoordinateSpace::World ) ? Vector::UnitZ : m_manipulationTransform.GetAxisZ();
                m_manipulationMode = ManipulationMode::RotateZ;
            }
            // Use the world space transform for screen space rotations even if we are in object space
            else if ( m_isScreenRotationWidgetHovered )
            {
                m_originalStartRotation = m_pTargetTransform->GetRotation();
                m_rotationAxis = viewport.GetViewForwardDirection();
                m_manipulationMode = ManipulationMode::RotateScreen;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_rotationStartMousePosition = Float2::Zero;
            m_manipulationMode = ManipulationMode::None;
        }
    }

    void Gizmo::Rotation_PerformManipulation( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();

        if ( m_manipulationMode == ManipulationMode::None )
        {
            return;
        }

        if ( io.MouseDownDuration[0] > 0 )
        {
            Vector const mouseDelta( io.MouseDelta );
            Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );

            if ( m_manipulationMode == ManipulationMode::RotateScreen )
            {
                // Calculate rotation angle
                Vector const originalScreenDir = Vector( m_rotationStartMousePosition - m_origin_SS ).GetNormalized2();
                Vector const desiredRotationScreenDir = ( mousePos - m_origin_SS ).GetNormalized2();
                m_rotationDeltaAngle = Math::GetAngleBetweenVectors( originalScreenDir, desiredRotationScreenDir );

                // Adjust direction of rotation
                bool isRotationDirectionPositive = Vector::Cross3( originalScreenDir, desiredRotationScreenDir ).m_z > 0.0f;
                if ( !isRotationDirectionPositive )
                {
                    m_rotationDeltaAngle = -m_rotationDeltaAngle;
                }
            }
            else
            {
                Plane rotationPlane = Plane::FromNormalAndPoint( m_rotationAxis, m_origin_WS );

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

                m_rotationDeltaAngle = Math::GetAngleBetweenVectors( originalVector_WS, newVector_WS );
                bool isRotationDirectionPositive = Vector::Dot3( Vector::Cross3( originalVector_WS, newVector_WS ).GetNormalized3(), m_rotationAxis ).ToFloat() > 0.0f;
                if ( !isRotationDirectionPositive )
                {
                    m_rotationDeltaAngle = -m_rotationDeltaAngle;
                }

            }

            //-------------------------------------------------------------------------

            if ( !Math::IsNearZero( (float) m_rotationDeltaAngle, (float) Degrees( 1.0f ).ToRadians() ) )
            {
                // Calculate rotation delta and apply it
                Quaternion const rotationDelta( m_rotationAxis, m_rotationDeltaAngle );
                m_pTargetTransform->SetRotation( m_originalStartRotation * rotationDelta );
            }
            else
            {
                m_rotationDeltaAngle = 0.0f;
            }
        }
    }

    void Gizmo::Rotation_Update( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Rotation );

        auto pDrawList = ImGui::GetWindowDrawList();

        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();

        //-------------------------------------------------------------------------

        if ( m_manipulationMode == ManipulationMode::None )
        {
            m_isScreenRotationWidgetHovered = Rotation_DrawScreenGizmo( viewport );

            Color elementColor = m_isAxisRotationWidgetHoveredX ? g_selectedColor : Colors::Red;
            m_isAxisRotationWidgetHoveredX = Rotation_DrawWidget( viewport, m_axisDir_WS_X, elementColor );

            elementColor = m_isAxisRotationWidgetHoveredY ? g_selectedColor : Colors::Lime;
            m_isAxisRotationWidgetHoveredY = Rotation_DrawWidget( viewport, m_axisDir_WS_Y, elementColor );

            elementColor = m_isAxisRotationWidgetHoveredZ ? g_selectedColor : Colors::Blue;
            m_isAxisRotationWidgetHoveredZ = Rotation_DrawWidget( viewport, m_axisDir_WS_Z, elementColor );

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
            if ( m_manipulationMode == ManipulationMode::RotateX )
            {
                Rotation_DrawManipulationWidget( viewport, m_axisDir_WS_X, m_axisDir_SS_X, Colors::Red );
            }
            else if ( m_manipulationMode == ManipulationMode::RotateY )
            {
                Rotation_DrawManipulationWidget( viewport, m_axisDir_WS_Y, m_axisDir_SS_Y, Colors::Lime );
            }
            else if ( m_manipulationMode == ManipulationMode::RotateZ )
            {
                Rotation_DrawManipulationWidget( viewport, m_axisDir_WS_Z, m_axisDir_SS_Z, Colors::Blue );
            }
            else if ( m_manipulationMode == ManipulationMode::RotateScreen )
            {
                Rotation_DrawManipulationWidget( viewport, viewport.GetViewForwardDirection(), Vector::UnitZ, Colors::White );
            }
        }

        Rotation_UpdateMode( viewport );
        Rotation_PerformManipulation( viewport );
    }

    //-------------------------------------------------------------------------

    bool Gizmo::Translation_DrawPlaneWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Translation );

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

    bool Gizmo::Translation_DrawAxis( Vector const& start, Vector const& end, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Translation );

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

    void Gizmo::Translation_UpdateMode( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Translation );

        ImGuiIO& io = ImGui::GetIO();
        if ( !viewport.ContainsPointScreenSpace( io.MousePos ) )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( io.MouseClicked[0]  )
        {
            Vector projectedPoint;
            Vector const& origin = m_manipulationTransform.GetTranslation();
            Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );
            LineSegment const mouseWorldRay = viewport.ScreenSpaceToWorldSpace( mousePos );

            if ( m_isAxisHoveredX )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisX() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateX;
            }
            else if ( m_isAxisHoveredY )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisY() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateY;
            }
            else if ( m_isAxisHoveredZ )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisZ() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_translationOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateZ;
            }
            else if ( m_isPlaneHoveredXY )
            {
                Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisZ(), origin );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateXY;
            }
            else if ( m_isPlaneHoveredXZ )
            {
                Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisY(), origin );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateXZ;
            }
            else if ( m_isPlaneHoveredYZ )
            {
                Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisX(), origin );
                if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    m_translationOffset = projectedPoint - origin;
                }

                m_manipulationMode = ManipulationMode::TranslateYZ;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_translationOffset = Vector::Zero;
            m_manipulationMode = ManipulationMode::None;
        }
    }

    void Gizmo::Translation_PerformManipulation( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Translation );

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
                if ( m_manipulationMode == ManipulationMode::TranslateX )
                {
                    Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisX() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_manipulationMode == ManipulationMode::TranslateY )
                {
                    Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisY() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_manipulationMode == ManipulationMode::TranslateZ )
                {
                    Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewForwardDirection(), origin );
                    if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisZ() );
                        Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                        translationDelta = projectedTranslation - origin - m_translationOffset;
                    }
                }

                else if ( m_manipulationMode == ManipulationMode::TranslateXY )
                {
                    Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisZ(), origin );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }

                else if ( m_manipulationMode == ManipulationMode::TranslateXZ )
                {
                    Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisY(), origin );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }

                else if ( m_manipulationMode == ManipulationMode::TranslateYZ )
                {
                    Plane translationPlane = Plane::FromNormalAndPoint( m_manipulationTransform.GetAxisX(), origin );
                    if ( translationPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                    {
                        translationDelta = ( projectedPoint - origin - m_translationOffset );
                    }
                }
            }

            m_pTargetTransform->SetTranslation( origin + translationDelta );
        }
    }

    void Gizmo::Translation_Update( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Translation );

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

        if ( m_manipulationMode == ManipulationMode::TranslateX )
        {
            drawAxisGuide( m_axisDir_SS_X, Colors::Red );
        }
        else if ( m_manipulationMode == ManipulationMode::TranslateY )
        {
            drawAxisGuide( m_axisDir_SS_Y, Colors::Lime );
        }
        else if ( m_manipulationMode == ManipulationMode::TranslateZ )
        {
            drawAxisGuide( m_axisDir_SS_Z, Colors::Blue );
        }

        pDrawList->AddCircleFilled( m_origin_SS.ToFloat2(), g_originSphereRadius, Colors::White );

        //-------------------------------------------------------------------------
        // Draw axes
        //-------------------------------------------------------------------------

        // If any axes face away from the camera negate them before drawing
        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();
        Vector const drawAxisDir_SS_X = m_axisDir_SS_X;
        Vector const drawAxisDir_SS_Y = m_axisDir_SS_Y;
        Vector const drawAxisDir_SS_Z = m_axisDir_SS_Z;

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
            elementColor = ( m_manipulationMode == ManipulationMode::TranslateX || m_manipulationMode == ManipulationMode::TranslateXY || m_manipulationMode == ManipulationMode::TranslateXZ || m_isAxisHoveredX ) ? g_selectedColor : Colors::Red;
            m_isAxisHoveredX = Translation_DrawAxis( m_origin_SS, axisEndPoint_SS_X, elementColor );
        }

        if ( shouldDrawAxis_Y )
        {
            elementColor = ( m_manipulationMode == ManipulationMode::TranslateY || m_manipulationMode == ManipulationMode::TranslateXY || m_manipulationMode == ManipulationMode::TranslateYZ || m_isAxisHoveredY ) ? g_selectedColor : Colors::Green;
            m_isAxisHoveredY = Translation_DrawAxis( m_origin_SS, axisEndPoint_SS_Y, elementColor );
        }

        if ( shouldDrawAxis_Z )
        {
            elementColor = ( m_manipulationMode == ManipulationMode::TranslateZ || m_manipulationMode == ManipulationMode::TranslateXZ || m_manipulationMode == ManipulationMode::TranslateYZ || m_isAxisHoveredZ ) ? g_selectedColor : Colors::Blue;
            m_isAxisHoveredZ = Translation_DrawAxis( m_origin_SS, axisEndPoint_SS_Z, elementColor );
        }

        //-------------------------------------------------------------------------
        // Draw planes
        //-------------------------------------------------------------------------

        elementColor = ( m_isPlaneHoveredXY || m_manipulationMode == ManipulationMode::TranslateXY ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredXY = Translation_DrawPlaneWidget( m_origin_SS, drawAxisDir_SS_X, drawAxisDir_SS_Y, elementColor );

        elementColor = ( m_isPlaneHoveredXZ || m_manipulationMode == ManipulationMode::TranslateXZ ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredXZ = Translation_DrawPlaneWidget( m_origin_SS, drawAxisDir_SS_X, drawAxisDir_SS_Z, elementColor );

        elementColor = ( m_isPlaneHoveredYZ || m_manipulationMode == ManipulationMode::TranslateYZ ) ? g_selectedColor : g_planeColor;
        m_isPlaneHoveredYZ = Translation_DrawPlaneWidget( m_origin_SS, drawAxisDir_SS_Y, drawAxisDir_SS_Z, elementColor );

        //-------------------------------------------------------------------------
        // Switch Mode and calculate the translation delta
        //-------------------------------------------------------------------------

        Translation_UpdateMode( viewport );
        Translation_PerformManipulation( viewport );
    }

    //-------------------------------------------------------------------------

    bool Gizmo::Scale_DrawOrigin( Vector const& originPosition )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

        ImGuiIO& io = ImGui::GetIO();
        auto pDrawList = ImGui::GetWindowDrawList();
        KRG_ASSERT( pDrawList != nullptr );

        uint32 const originColor = ConvertColor( ( m_manipulationMode == ManipulationMode::ScaleXYZ || m_isOriginHovered ) ? g_selectedColor : Colors::White );

        pDrawList->AddCircleFilled( originPosition.ToFloat2(), 3.0f, originColor, 20 );
        pDrawList->AddCircle( originPosition.ToFloat2(), 8.0f, originColor, 20, 2.0f );

        if ( m_isOriginHovered || m_manipulationMode == ManipulationMode::ScaleXYZ )
        {
            pDrawList->AddCircle( originPosition.ToFloat2(), g_selectedOriginSphereRadius, originColor, 12, 2.0f );
        }

        //-------------------------------------------------------------------------

        Vector const mousePos = io.MousePos;
        float const mouseToOriginDistance = originPosition.GetDistance2( mousePos );
        bool const isHovered = mouseToOriginDistance < g_originSphereRadius;
        return isHovered;
    }

    bool Gizmo::Scale_DrawAxis( Vector const& start, Vector const& end, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

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

    bool Gizmo::Scale_DrawPlaneWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

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

    void Gizmo::Scale_UpdateMode( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

        ImGuiIO& io = ImGui::GetIO();
        if ( !viewport.ContainsPointScreenSpace( io.MousePos ) )
        {
            return;
        }

        //-------------------------------------------------------------------------

        Vector const& origin = m_manipulationTransform.GetTranslation();
        Vector const mousePos( io.MousePos.x, io.MousePos.y, 0, 1.0f );
        LineSegment const mouseWorldRay = viewport.ScreenSpaceToWorldSpace( mousePos );
        Vector projectedPoint;

        if ( io.MouseClicked[0] )
        {
            if ( m_isOriginHovered )
            {
                m_manipulationMode = ManipulationMode::ScaleXYZ;
            }
            else if ( m_isAxisHoveredX )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewVolume().GetForwardVector(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisX() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::ScaleX;
            }
            else if ( m_isAxisHoveredY )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewVolume().GetForwardVector(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisY() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::ScaleY;
            }
            else if ( m_isAxisHoveredZ )
            {
                Plane const viewPlane = Plane::FromNormalAndPoint( viewport.GetViewVolume().GetForwardVector(), origin );
                if ( viewPlane.IntersectLine( mouseWorldRay.GetStartPoint(), mouseWorldRay.GetEndPoint(), projectedPoint ) )
                {
                    LineSegment const manipulationAxis( origin, origin + m_manipulationTransform.GetAxisZ() );
                    Vector const projectedTranslation = manipulationAxis.GetClosestPointOnLine( projectedPoint );
                    m_positionOffset = projectedTranslation - origin;
                }

                m_manipulationMode = ManipulationMode::ScaleZ;
            }
        }
        else if ( io.MouseReleased[0] )
        {
            m_positionOffset = Vector::Zero;
            m_manipulationMode = ManipulationMode::None;
        }
    }

    void Gizmo::Scale_PerformManipulation( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

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

                if ( m_manipulationMode == ManipulationMode::ScaleXYZ )
                {
                    auto const axisSS = viewport.WorldSpaceToScreenSpace( originWS + m_manipulationTransform.GetAxisZ() );
                    Vector const unitsPerPixel = Vector::One / Vector( axisSS - originSS ).Length2();
                    scaleDelta = Vector::One * -mouseDelta.m_y * unitsPerPixel;
                }

                else if ( m_manipulationMode == ManipulationMode::ScaleX || m_manipulationMode == ManipulationMode::ScaleY || m_manipulationMode == ManipulationMode::ScaleZ )
                {
                    Vector scaleDeltaAxis;

                    if ( m_manipulationMode == ManipulationMode::ScaleX )
                    {
                        manipulationAxis = m_isAxisNegatedX ? -m_manipulationTransform.GetAxisX() : m_manipulationTransform.GetAxisX();
                        scaleDeltaAxis = Vector::UnitX;
                    }
                    else if ( m_manipulationMode == ManipulationMode::ScaleY )
                    {
                        manipulationAxis = m_isAxisNegatedY ? -m_manipulationTransform.GetAxisY() : m_manipulationTransform.GetAxisY();
                        scaleDeltaAxis = Vector::UnitY;
                    }
                    else if ( m_manipulationMode == ManipulationMode::ScaleZ )
                    {
                        manipulationAxis = m_isAxisNegatedZ ? -m_manipulationTransform.GetAxisZ() : m_manipulationTransform.GetAxisZ();
                        scaleDeltaAxis = Vector::UnitZ;
                    }

                    // If we are in world we need to calculate the scale delta axis
                    if ( m_coordinateSpace == CoordinateSpace::World )
                    {
                        Vector const localAxisX = m_pTargetTransform->GetAxisX();
                        Vector const localAxisY = m_pTargetTransform->GetAxisY();
                        Vector const localAxisZ = m_pTargetTransform->GetAxisZ();

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
                m_pTargetTransform->SetScale( m_pTargetTransform->GetScale() * scaleDelta );
            }
        }
    }

    void Gizmo::Scale_Update( Render::Viewport const& viewport )
    {
        KRG_ASSERT( m_gizmoMode == GizmoMode::Scale );

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

        if ( m_manipulationMode == ManipulationMode::ScaleX )
        {
            drawAxisGuide( m_axisDir_SS_X, Colors::Red );
        }
        else if ( m_manipulationMode == ManipulationMode::ScaleY )
        {
            drawAxisGuide( m_axisDir_SS_Y, Colors::Lime );
        }
        else if ( m_manipulationMode == ManipulationMode::ScaleZ )
        {
            drawAxisGuide( m_axisDir_SS_Z, Colors::Blue );
        }

        m_isOriginHovered = Scale_DrawOrigin( m_origin_SS );

        //-------------------------------------------------------------------------
        // Draw axes
        //-------------------------------------------------------------------------

        // If any axes face away from the camera negate them before drawing
        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();

        Vector const drawAxisDir_SS_X = m_axisDir_SS_X;
        Vector const drawAxisDir_SS_Y = m_axisDir_SS_Y;
        Vector const drawAxisDir_SS_Z = m_axisDir_SS_Z;

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
            elementColor = ( m_manipulationMode == ManipulationMode::ScaleX || m_isAxisHoveredX ) ? g_selectedColor : Colors::Red;
            m_isAxisHoveredX = Scale_DrawAxis( m_origin_SS, axisEndPoint_SS_X, elementColor );
        }

        if ( shouldDrawAxis_Y )
        {
            elementColor = ( m_manipulationMode == ManipulationMode::ScaleY || m_isAxisHoveredY ) ? g_selectedColor : Colors::Lime;
            m_isAxisHoveredY = Scale_DrawAxis( m_origin_SS, axisEndPoint_SS_Y, elementColor );
        }

        if ( shouldDrawAxis_Z )
        {
            elementColor = ( m_manipulationMode == ManipulationMode::ScaleZ || m_isAxisHoveredZ ) ? g_selectedColor : Colors::Blue;
            m_isAxisHoveredZ = Scale_DrawAxis( m_origin_SS, axisEndPoint_SS_Z, elementColor );
        }

        //-------------------------------------------------------------------------
        // Switch Mode and calculate the translation delta
        //-------------------------------------------------------------------------

        Scale_UpdateMode( viewport );
        Scale_PerformManipulation( viewport );
    }
}
#endif