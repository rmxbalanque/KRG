#include "ImguiFont.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    ImFont* SystemFonts::s_fonts[(S32) Font::NumFonts] = { nullptr, nullptr, nullptr, nullptr };

    //-------------------------------------------------------------------------

    ScopedFont::ScopedFont( Font font )
    {
        KRG_ASSERT( font != Font::NumFonts && SystemFonts::s_fonts[(U8) font] != nullptr );
        ::ImGui::PushFont( SystemFonts::s_fonts[(U8) font] );
    }

    ScopedFont::~ScopedFont()
    {
        ::ImGui::PopFont();
    }
}
#endif