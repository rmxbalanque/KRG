#pragma once

#include "_Module/API.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_internal.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/Time.h"

#if _WIN32
#include "System/Platform/ImguiSystem_win32.inl"
#endif

//-------------------------------------------------------------------------
// ImGui Extensions
//-------------------------------------------------------------------------
// This is the primary integration of DearImgui in KRG.
//
// * Provides the necessary imgui state updates through the frame start/end functions
// * Provides helpers for common operations
//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace ImGuiX
    {
        //-------------------------------------------------------------------------
        // Fonts
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_IMGUI_API ScopedFont
        {
        public:

            ScopedFont( Font font );
            ~ScopedFont();
        };

        //-------------------------------------------------------------------------
        // Helpers
        //-------------------------------------------------------------------------

        inline ImVec4 ConvertColor( KRG::Color const& color )
        {
            return ::ImGui::ColorConvertU32ToFloat4( IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a ) );
        }

        inline ImU32 ConvertColorU32( KRG::Color const& color )
        {
            return IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a );
        }
    }
}
#endif