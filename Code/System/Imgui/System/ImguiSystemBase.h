#pragma once

#include "../_Module/API.h"
#include "System/Imgui/Renderer/ImguiRenderer.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ImGuiX
    {
        enum class Font : U8
        {
            Small = 0,
            Medium,
            Large,

            NumFonts,
            Default = Medium,
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_IMGUI_API SystemBase
        {

        public:

            static ImFont*          SystemFonts[(U8) Font::NumFonts];

        public:

            void Update( float deltaTime );

        protected:

            bool Initialize();
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

            ImGuiMouseCursor        m_lastMouseCursorState = ImGuiMouseCursor_COUNT;
        };
    }
}