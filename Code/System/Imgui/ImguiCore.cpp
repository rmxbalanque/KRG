#include "ImguiCore.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace ImGuiX
    {
        ScopedFont::ScopedFont( Font font )
        {
            KRG_ASSERT( font != Font::NumFonts && SystemBase::SystemFonts[(U8) font] != nullptr );
            ::ImGui::PushFont( SystemBase::SystemFonts[(U8) font] );
        }

        ScopedFont::~ScopedFont()
        {
            ::ImGui::PopFont();
        }

        //-------------------------------------------------------------------------
    }
}
#endif