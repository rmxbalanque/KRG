#include "ImguiFont.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    ImFont* SystemFonts::s_fonts[(int32) Font::NumFonts] = { nullptr, nullptr, nullptr, nullptr };

    //-------------------------------------------------------------------------

    ScopedFont::ScopedFont( Font font )
    {
        KRG_ASSERT( font != Font::NumFonts && SystemFonts::s_fonts[(uint8) font] != nullptr );
        ImGui::PushFont( SystemFonts::s_fonts[(uint8) font] );
    }

    ScopedFont::ScopedFont( Font font, ImColor const& color )
    {
        KRG_ASSERT( font != Font::NumFonts && SystemFonts::s_fonts[(uint8) font] != nullptr );
        ImGui::PushFont( SystemFonts::s_fonts[(uint8) font] );

        ImGui::PushStyleColor( ImGuiCol_Text, color.Value );
        m_colorApplied = true;
    }

    ScopedFont::~ScopedFont()
    {
        if ( m_colorApplied )
        {
            ImGui::PopStyleColor();
        }

        ImGui::PopFont();
    }
}
#endif