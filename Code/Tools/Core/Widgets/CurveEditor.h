#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Render/ThirdParty/imgui/imgui.h"
#include "System/Core/Math/FloatCurve.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------
// Curve Editor
//-------------------------------------------------------------------------
// Controls:
//  hold middle mouse - pan view
//  mouse wheel - zoom view
//  ctrl + mouse wheel - zoom horizontal view
//  alt + mouse wheel - zoom vertical views
//  hold shift - show cursor position tooltip

namespace KRG
{
    struct CurveEditorContext;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API CurveEditor
    {
        constexpr static float const s_fitViewExtraMarginPercentage = 0.05f;
        constexpr static float const s_handleRadius = 6.0f;
        constexpr static float const s_zoomScaleStep = 0.1f;
        constexpr static float const s_pixelsPerGridBlock = 45;
        constexpr static float const s_slopeHandleLength = 45.0f;
        constexpr static float const s_gridLegendWidth = 30;
        constexpr static float const s_gridLegendHeight = 16;

        constexpr static char const* const s_pointContextMenuName = "PointCtxMenu";
        constexpr static char const* const s_gridContextMenuName = "GridCtxMenu";

        static ImColor const s_curveColor;
        static ImColor const s_curvePointHandleColor;
        static ImColor const s_curveSelectedPointHandleColor;
        static ImColor const s_curveInTangentHandleColor;
        static ImColor const s_curveOutTangentHandleColor;

    public:

        CurveEditor( FloatCurve& curve );

        // Draw the the curve editor and edit the curve
        void UpdateAndDraw();

        // Adjust view range so that the entire curve is visible
        void ViewEntireCurve();

        // Adjust view range so that the entire curve parameter range is visible
        void ViewEntireHorizontalRange();

        // Adjust view range so that the entire curve value range is visible
        void ViewEntireVerticalRange();

    private:

        void CreatePoint( float parameter, float value );
        void DeletePoint( int32 pointIdx );

        //-------------------------------------------------------------------------

        void InitializeFrameState();

        void DrawToolbar();
        void DrawGrid();
        void DrawCurve();
        void DrawInTangentHandle( int32 pointIdx );
        void DrawOutTangentHandle( int32 pointIdx );
        void DrawPointHandle( int32 pointIdx );
        void DrawContextMenus();
        void HandleFrameInput();

        inline Float2 GetCurvePosFromScreenPos( Float2 const& screenPos ) const
        {
            ImVec2 curvePos( screenPos - m_curveCanvasStart );
            curvePos.x /= m_curveCanvasWidth;
            curvePos.y /= m_curveCanvasHeight;
            curvePos.x = m_horizontalViewRange.GetValueForPercentageThrough( curvePos.x );
            curvePos.y = m_verticalViewRange.GetValueForPercentageThrough( 1.0f - curvePos.y );
            return curvePos;
        }

        inline Float2 GetScreenPosFromCurvePos( float parameter, float value ) const
        {
            Float2 screenPos;
            screenPos.m_x = m_curveCanvasStart.m_x + ( m_horizontalViewRange.GetPercentageThrough( parameter ) * m_curveCanvasWidth );
            screenPos.m_y = m_curveCanvasEnd.m_y - ( m_verticalViewRange.GetPercentageThrough( value ) * m_curveCanvasHeight );
            return screenPos;
        }

        KRG_FORCE_INLINE Float2 GetScreenPosFromCurvePos( Float2 const& curvePos ) const { return GetScreenPosFromCurvePos( curvePos.m_x, curvePos.m_y ); }
        KRG_FORCE_INLINE Float2 GetScreenPosFromCurvePos( FloatCurve::Point const& point ) const { return GetScreenPosFromCurvePos( point.m_parameter, point.m_value ); }

    private:

        // Persistent State
        FloatCurve&         m_curve;
        FloatRange          m_horizontalViewRange = FloatRange( 0, 1 );     // The horizontal grid range
        FloatRange          m_verticalViewRange = FloatRange( 0, 1 );       // The vertical grid range
        int32               m_selectedPointIdx = InvalidIndex;

        // Per Frame State
        ImDrawList*         m_pDrawList = nullptr;

        Float2              m_windowPos;
        Float2              m_canvasStart;
        Float2              m_canvasEnd;
        float               m_canvasWidth;
        float               m_canvasHeight;

        Float2              m_curveCanvasStart;
        Float2              m_curveCanvasEnd;
        float               m_curveCanvasWidth;
        float               m_curveCanvasHeight;

        float               m_horizontalRangeLength;
        float               m_verticalRangeLength;
        float               m_pixelsPerUnitHorizontal;
        float               m_pixelsPerUnitVertical;
        bool                m_wasPointClicked = false;
    };
}