#pragma once

#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    struct VisualSettings
    {
        // Colors
        //-------------------------------------------------------------------------

        constexpr static uint32 const   s_gridBackgroundColor = IM_COL32( 40, 40, 40, 200 );
        constexpr static uint32 const   s_gridLineColor = IM_COL32( 200, 200, 200, 40 );
        constexpr static uint32 const   s_graphTitleColor = IM_COL32( 255, 255, 255, 255 );
        constexpr static uint32 const   s_selectionBoxOutlineColor = IM_COL32( 61, 133, 224, 30 );
        constexpr static uint32 const   s_selectionBoxFillColor = IM_COL32( 61, 133, 224, 150 );

        constexpr static uint32 const   s_genericNodeTitleTextColor = IM_COL32( 0, 0, 0, 255 );
        constexpr static uint32 const   s_genericNodeTitleColor = IM_COL32( 28, 28, 28, 255 );
        constexpr static uint32 const   s_genericNodeBackgroundColor = IM_COL32( 75, 75, 75, 255 );
        constexpr static uint32 const   s_genericNodeBorderColorSelected = IM_COL32( 255, 255, 255, 255 );
        constexpr static uint32 const   s_genericNodeBackgroundColorSelected = IM_COL32( 95, 95, 95, 255 );
        constexpr static uint32 const   s_genericHoverColor = IM_COL32( 200, 200, 200, 255 );
        constexpr static uint32 const   s_genericSelectionColor = IM_COL32( 255, 255, 0, 255 );

        constexpr static uint32 const   s_connectionColor = IM_COL32( 185, 185, 185, 255 );
        constexpr static uint32 const   s_connectionColorValid = IM_COL32( 0, 255, 0, 255 );
        constexpr static uint32 const   s_connectionColorInvalid = IM_COL32( 255, 0, 0, 255 );
        constexpr static uint32 const   s_connectionColorHovered = IM_COL32( 255, 255, 255, 255 );

        // UI
        //-------------------------------------------------------------------------

        static ImVec2 const             s_graphTitleMargin;
        constexpr static float const    s_gridSpacing = 20;
        constexpr static float const    s_connectionSelectionExtraRadius = 5.0f;
    };

    //-------------------------------------------------------------------------
    // Drawing Context
    //-------------------------------------------------------------------------

    struct DrawingContext
    {
        KRG_FORCE_INLINE bool IsMouseInViewWindow() const
        {
            return m_windowRect.Contains( m_mouseScreenPos );
        }

        // Convert from a position relative to the window TL to a position relative to the screen TL
        KRG_FORCE_INLINE ImVec2 WindowToScreenPosition( ImVec2 const& windowPosition ) const
        {
            return windowPosition + m_windowRect.Min;
        }

        // Convert from a position relative to the window TL to a position relative to the canvas origin
        KRG_FORCE_INLINE ImVec2 WindowPositionToCanvasPosition( ImVec2 const& windowPosition ) const
        {
            return windowPosition + m_viewOffset;
        }

        // Convert from a position relative to the canvas origin to a position relative to the window TL
        KRG_FORCE_INLINE ImVec2 CanvasPositionToWindowPosition( ImVec2 const& canvasPosition ) const
        {
            return canvasPosition - m_viewOffset;
        }

        // Convert from a position relative to the canvas origin to a position relative to the screen TL
        KRG_FORCE_INLINE ImVec2 CanvasPositionToScreenPosition( ImVec2 const& canvasPosition ) const
        {
            return WindowToScreenPosition( CanvasPositionToWindowPosition( canvasPosition ) );
        }

        // Convert from a position relative to the screen TL to a position relative to the window TL
        KRG_FORCE_INLINE ImVec2 ScreenPositionToWindowPosition( ImVec2 const& screenPosition ) const
        {
            return screenPosition - m_windowRect.Min;
        }

        // Convert from a position relative to the screen TL to a position relative to the canvas origin
        KRG_FORCE_INLINE ImVec2 ScreenPositionToCanvasPosition( ImVec2 const& screenPosition ) const
        {
            return WindowPositionToCanvasPosition( ScreenPositionToWindowPosition( screenPosition ) );
        }

        // Is a supplied rect within the canvas visible area
        KRG_FORCE_INLINE bool IsItemVisible( ImRect const& itemCanvasRect ) const
        {
            return m_canvasVisibleRect.Overlaps( itemCanvasRect );
        }

    public:

        ImDrawList*         m_pDrawList = nullptr;
        ImVec2              m_viewOffset = ImVec2( 0, 0 );
        ImRect              m_windowRect;
        ImRect              m_canvasVisibleRect;
        ImVec2              m_mouseScreenPos = ImVec2( 0, 0 );
        ImVec2              m_mouseCanvasPos = ImVec2( 0, 0 );

        // User data to provide extra information with which to draw the extra node controls
        void*               m_pUserContext = nullptr;
    };
}