#pragma once

#include "../_Module/API.h"
#include "ImguiFont.h"
#include "ImguiStyle.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// ImGui Extensions
//-------------------------------------------------------------------------
// This is the primary integration of DearImgui in KRG.
//
// * Provides the necessary imgui state updates through the frame start/end functions
// * Provides helpers for common operations
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    //-------------------------------------------------------------------------
    // Color Helpers
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE ImColor ConvertColor( Color const& color )
    {
        return IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a );
    }

    // Adjust the brightness of color by a multiplier
    KRG_FORCE_INLINE ImColor AdjustColorBrightness( ImColor const& color, float multiplier )
    {
        Float4 tmpColor = (ImVec4) color;
        float const alpha = tmpColor.m_w;
        tmpColor *= multiplier;
        tmpColor.m_w = alpha;
        return ImColor( tmpColor );
    }

    // Adjust the brightness of color by a multiplier
    KRG_FORCE_INLINE ImU32 AdjustColorBrightness( ImU32 color, float multiplier )
    {
        return AdjustColorBrightness( ImColor( color ), multiplier );
    }

    //-------------------------------------------------------------------------
    // Docking helpers
    //-------------------------------------------------------------------------

    inline void MakeTabVisible( char const* const pWindowName )
    {
        KRG_ASSERT( pWindowName != nullptr );
        ImGuiWindow* pWindow = ImGui::FindWindowByName( pWindowName );
        if ( pWindow == nullptr || pWindow->DockNode == nullptr || pWindow->DockNode->TabBar == nullptr )
        {
            return;
        }

        pWindow->DockNode->TabBar->NextSelectedTabId = pWindow->ID;
    }

    //-------------------------------------------------------------------------
    // Widgets
    //-------------------------------------------------------------------------

    inline void ItemTooltip( const char* fmt, ... )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::IsItemHovered() && GImGui->HoveredIdTimer > Style::s_toolTipDelay )
        {
            va_list args;
            va_start( args, fmt );
            ImGui::SetTooltipV( fmt, args );
            va_end( args );
        }
        ImGui::PopStyleVar();
    }

    inline void ItemTooltipDelayed( float tooltipDelay, const char* fmt, ... )
    {
        KRG_ASSERT( tooltipDelay > 0 );
        if ( ImGui::IsItemHovered() && GImGui->HoveredIdTimer > tooltipDelay )
        {
            va_list args;
            va_start( args, fmt );
            ImGui::SetTooltipV( fmt, args );
            va_end( args );
        }
    }

    inline void SelectableText( String const& value, float width = 0 )
    {
        if ( width != 0 )
        {
            ImGui::SetNextItemWidth( width );
        }
        ImGui::InputText( "##", const_cast<char*>( value.c_str() ), value.length(), ImGuiInputTextFlags_ReadOnly );
    }

    inline bool ButtonColored( ImVec4 const& foregroundColor, char const* label, ImVec2 const& size = ImVec2( 0, 0 ) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, foregroundColor );
        bool const result = ImGui::Button( label, size );
        ImGui::PopStyleColor();

        return result;
    }

    KRG_SYSTEM_RENDER_API ImVec2 const& GetClosestPointOnRect( ImRect const& rect, ImVec2 const& inPoint );

    // Draw an arrow between two points
    KRG_SYSTEM_RENDER_API void DrawArrow( ImDrawList* pDrawList, ImVec2 const& arrowStart, ImVec2 const& arrowEnd, ImU32 col, float arrowWidth, float arrowHeadWidth = 5.0f );

    // Draw an overlaid icon in a window, returns true if clicked
    KRG_SYSTEM_RENDER_API bool DrawOverlayIcon( ImVec2 const& iconPos, char icon[4], void* iconID );

    //-------------------------------------------------------------------------
    // Separators
    //-------------------------------------------------------------------------

    // Create a centered separator which can be immediately followed by a item
    KRG_SYSTEM_RENDER_API void PreSeparator( float width = 0 );

    // Create a centered separator which can be immediately followed by a item
    KRG_SYSTEM_RENDER_API void PostSeparator( float width = 0 );

    // Create a labeled separator: --- TEXT ---------------
    KRG_SYSTEM_RENDER_API void TextSeparator( char* text, float preWidth = 10.0f, float totalWidth = 0 );

    // Draws a vertical separator on the current line and forces the next item to be on the same line. The size is the offset between the previous item and the next
    KRG_SYSTEM_RENDER_API void VerticalSeparator( ImVec2 const& size = ImVec2( 9, -1 ), ImColor const& color = 0 );
}
#endif