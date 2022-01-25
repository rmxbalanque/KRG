#pragma once

#include "System/Render/_Module/API.h"
#include "ImguiFont.h"
#include "ImguiStyle.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "System/Core/Math/Transform.h"
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
    // General helpers
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

    KRG_SYSTEM_RENDER_API ImVec2 const& GetClosestPointOnRect( ImRect const& rect, ImVec2 const& inPoint );

    //-------------------------------------------------------------------------
    // Separators
    //-------------------------------------------------------------------------

    // Create a centered separator which can be immediately followed by a item
    KRG_SYSTEM_RENDER_API void PreSeparator( float width = 0 );

    // Create a centered separator which can be immediately followed by a item
    KRG_SYSTEM_RENDER_API void PostSeparator( float width = 0 );

    // Create a labeled separator: --- TEXT ---------------
    KRG_SYSTEM_RENDER_API void TextSeparator( char const* text, float preWidth = 10.0f, float totalWidth = 0 );

    // Draws a vertical separator on the current line and forces the next item to be on the same line. The size is the offset between the previous item and the next
    KRG_SYSTEM_RENDER_API void VerticalSeparator( ImVec2 const& size = ImVec2( -1, -1 ), ImColor const& color = 0 );

    //-------------------------------------------------------------------------
    // Widgets
    //-------------------------------------------------------------------------

    // Draw a tooltip for the immediately preceding item
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

    // Draw a tooltip with a custom hover delay for the immediately preceding item
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

    // Draw a colored button
    KRG_SYSTEM_RENDER_API bool ColoredButton( ImColor const& backgroundColor, ImColor const& foregroundColor, char const* label, ImVec2 const& size = ImVec2( 0, 0 ) );

    // Draw a colored button
    inline bool ColoredButton( Color backgroundColor, Color foregroundColor, char const* label, ImVec2 const& size = ImVec2( 0, 0 ) )
    {
        return ColoredButton( ConvertColor( backgroundColor ), ConvertColor( foregroundColor ), label, size );
    }

    // Draws a flat button - a button with no background
    KRG_SYSTEM_RENDER_API bool FlatButton( char const* label, ImVec2 const& size = ImVec2( 0, 0 ) );

    // Draw an arrow between two points
    KRG_SYSTEM_RENDER_API void DrawArrow( ImDrawList* pDrawList, ImVec2 const& arrowStart, ImVec2 const& arrowEnd, ImU32 col, float arrowWidth, float arrowHeadWidth = 5.0f );

    // Draw an overlaid icon in a window, returns true if clicked
    KRG_SYSTEM_RENDER_API bool DrawOverlayIcon( ImVec2 const& iconPos, char icon[4], void* iconID, bool isSelected = false, ImColor const& selectedColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] );

    // Draw a basic spinner
    KRG_SYSTEM_RENDER_API bool DrawSpinner( char const* pLabel, ImColor const& color = Style::s_textColor, float radius = 6.0f, float thickness = 3.0f );

    //-------------------------------------------------------------------------
    // Numeric Widgets
    //-------------------------------------------------------------------------

    KRG_SYSTEM_RENDER_API bool InputFloat2( char const* pID, Float2& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_RENDER_API bool InputFloat3( char const* pID, Float3& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_RENDER_API bool InputFloat4( char const* pID, Float4& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_RENDER_API bool InputFloat4( char const* pID, Vector& value, float width = -1, bool readOnly = false );

    KRG_SYSTEM_RENDER_API bool InputTransform( char const* pID, Transform& value, float width = -1, bool readOnly = false );

    //-------------------------------------------------------------------------

    KRG_SYSTEM_RENDER_API void DisplayVector2( ImGuiID ID, Vector const& v, float width = -1 );
    KRG_SYSTEM_RENDER_API void DisplayVector3( ImGuiID ID, Vector const& v, float width = -1 );
    KRG_SYSTEM_RENDER_API void DisplayVector4( ImGuiID ID, Vector const& v, float width = -1 );

    KRG_FORCE_INLINE void DisplayVector2( Vector const& v, float width = -1 ) { DisplayVector2( ImGui::GetID( &v ), v, width ); }
    KRG_FORCE_INLINE void DisplayVector3( Vector const& v, float width = -1 ) { DisplayVector3( ImGui::GetID( &v ), v, width ); }
    KRG_FORCE_INLINE void DisplayVector4( Vector const& v, float width = -1 ) { DisplayVector4( ImGui::GetID( &v ), v, width ); }
    
    KRG_FORCE_INLINE void DisplayVector( ImGuiID ID, Float2 const& v, float width = -1 ) { return DisplayVector2( ID, v, width ); }
    KRG_FORCE_INLINE void DisplayVector( ImGuiID ID, Float3 const& v, float width = -1 ) { return DisplayVector3( ID, v, width ); }
    KRG_FORCE_INLINE void DisplayVector( ImGuiID ID, Float4 const& v, float width = -1 ) { return DisplayVector4( ID, v, width ); }

    KRG_FORCE_INLINE void DisplayVector( Float2 const& v, float width = -1 ) { return DisplayVector2( v, width ); }
    KRG_FORCE_INLINE void DisplayVector( Float3 const& v, float width = -1 ) { return DisplayVector3( v, width ); }
    KRG_FORCE_INLINE void DisplayVector( Float4 const& v, float width = -1 ) { return DisplayVector4( v, width ); }

    KRG_SYSTEM_RENDER_API void DisplayRotation( ImGuiID ID, Quaternion const& q, float width = -1 );
    KRG_FORCE_INLINE void DisplayRotation( Quaternion const& q, float width = -1 ) { DisplayRotation( ImGui::GetID( &q ), q, width ); }

    KRG_SYSTEM_RENDER_API void DisplayTransform( ImGuiID ID, Transform const& t, float width = -1 );
    KRG_FORCE_INLINE void DisplayTransform( Transform const& t, float width = -1 ) { DisplayTransform( ImGui::GetID( &t ), t, width ); }
}
#endif