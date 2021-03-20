#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------
// Platform agnostic base ImGui integration
//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class KRG_SYSTEM_IMGUI_API ImguiSystemBase
    {

    public:

        void StartFrame( float deltaTime );
        void EndFrame();

    protected:

        bool Initialize( String const& settingsIniFilename );
        void Shutdown();

        // Platform Specific
        virtual void InitializePlatform() = 0;
        virtual void UpdateDisplayInformation() = 0;
        virtual void UpdateKeyStates() = 0;
        virtual void UpdateMousePosition() = 0;
        virtual bool UpdateMouseCursor() = 0;

    private:

        void InitializeFonts();
        void InitializeStyle();

    protected:

        bool                    m_updateMonitorInfo = true;

    private:

        String                  m_iniFilename;
        ImGuiMouseCursor        m_lastMouseCursorState = ImGuiMouseCursor_COUNT;
    };
}