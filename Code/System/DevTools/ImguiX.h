#pragma once

#include "_Module/API.h"
#include "System/ImguiFont.h"
#include "System/ImguiTheme.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_internal.h"
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
    // Helpers
    //-------------------------------------------------------------------------

    inline ImVec4 ConvertColor( Color const& color )
    {
        return ::ImGui::ColorConvertU32ToFloat4( IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a ) );
    }

    inline ImU32 ConvertColorU32( Color const& color )
    {
        return IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a );
    }

    //-------------------------------------------------------------------------

    inline void ItemTooltip( const char* fmt, ... )
    {
        if ( ImGui::IsItemHovered() && GImGui->HoveredIdTimer > Theme::s_toolTipDelay )
        {
            va_list args;
            va_start( args, fmt );
            ImGui::SetTooltipV( fmt, args );
            va_end( args );
        }
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
}
#endif