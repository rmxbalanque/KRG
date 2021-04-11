#pragma once

#include "_Module/API.h"
#include "System/ImguiFont.h"
#include "System/ImguiTheme.h"
#include "ThirdParty/imgui/imgui.h"
#include "System/Core/Types/Color.h"

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

    inline bool ButtonColored( char const* label, Color const& foregroundColor, ImVec2 const& size = ImVec2( 0, 0 ) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, foregroundColor.ToFloat4() );
        bool const result = ImGui::Button( label, size );
        ImGui::PopStyleColor();

        return result;
    }
}
#endif