#pragma once

#include "_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------
// Base ImGui integration
//-------------------------------------------------------------------------

namespace KRG::Render { class RenderDevice; }

namespace KRG::ImGuiX
{
    class KRG_SYSTEM_IMGUI_API ImguiSystem
    {

    public:

        void StartFrame( float deltaTime );
        void EndFrame();

        bool Initialize( String const& settingsIniFilename, Render::RenderDevice* pRenderDevice, bool enableViewports = false );
        void Shutdown();

    private:

        void InitializeFonts();

    private:

        String                  m_iniFilename;
    };
}