#pragma once

#include "_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

#if _WIN32
#include "Platform/ImguiSystem.inl"
#endif

//-------------------------------------------------------------------------
// Base ImGui integration
//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class KRG_SYSTEM_IMGUI_API ImguiSystem
    {

    public:

        void StartFrame( float deltaTime );
        void EndFrame();

        bool Initialize( String const& settingsIniFilename );
        void Shutdown();

        // Platform Specific functions
        PlatformInputResult ProcessInput( PlatformInputData const& data );
        void InitializePlatform();
        void UpdateDisplayInformation();
        void UpdateMousePosition();
        bool UpdateMouseCursor();

    private:

        void InitializeFonts();

    protected:

        bool                    m_updateMonitorInfo = true;

    private:

        String                  m_iniFilename;
        ImGuiMouseCursor        m_lastMouseCursorState = ImGuiMouseCursor_COUNT;
        void*                   m_mainWindowHandle = nullptr;
    };
}