#include "ImguiSystemBase.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_ProggyTiny.h"
#include "System/Render/Fonts/FontData_PixelCarnage.h"
#include "System/Render/Fonts/FontData_Roboto.h"
#include "System/Render/Fonts/FontData_FontAwesome4.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ImGuiX
    {
        ImFont* SystemBase::SystemFonts[(U8) Font::NumFonts] = { nullptr };

        //-------------------------------------------------------------------------

        bool SystemBase::Initialize()
        {
            ImGui::CreateContext();

            //-------------------------------------------------------------------------

            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            //-------------------------------------------------------------------------

            InitializePlatform();
            InitializeFonts();
            InitializeStyle();

            return true;
        }

        void SystemBase::Shutdown()
        {
            for ( int i = 0; i < (S8) Font::NumFonts; i++ )
            {
                SystemFonts[i] = nullptr;
            }

            ImGui::DestroyContext();
        }

        void SystemBase::Update( float deltaTime )
        {
            ImGui::NewFrame();

            //-------------------------------------------------------------------------

            ImGuiIO& io = ImGui::GetIO();
            io.DeltaTime = deltaTime;

            //-------------------------------------------------------------------------

            UpdateDisplayInformation();
            UpdateKeyStates();
            UpdateMousePosition();

            //-------------------------------------------------------------------------

            ImGuiMouseCursor newMouseCursorState = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
            if ( m_lastMouseCursorState != newMouseCursorState )
            {
                m_lastMouseCursorState = newMouseCursorState;
                UpdateMouseCursor();
            }
        }

        void SystemBase::InitializeFonts()
        {
            ImGuiIO& io = ImGui::GetIO();

            //-------------------------------------------------------------------------

            TVector<Byte> fontAwesomeCompressedData, fontAwesomeData;
            Fonts::FontAwesome4::GetFontCompressedData( fontAwesomeCompressedData );
            Fonts::GetDecompressedFontData( fontAwesomeCompressedData.data(), fontAwesomeData );
            ImWchar const icons_ranges[] = { Fonts::FontAwesome4::ICON_RANGE_START, Fonts::FontAwesome4::ICON_RANGE_END, 0 };

            TVector<Byte> fontCompressedData, fontData;
            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = false;

            ImFontConfig iconFontConfig;
            iconFontConfig.FontDataOwnedByAtlas = false;
            iconFontConfig.MergeMode = true;
            iconFontConfig.PixelSnapH = true;

            // Create default fonts for IMGUI
            //-------------------------------------------------------------------------

            Fonts::GetDecompressedFontData( Fonts::ProggyTiny::GetFontCompressedData(), fontData );
            fontConfig.GlyphOffset = ImVec2( 0, 1 );
            ImFont* pSmallFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (S32) fontData.size(), 10, &fontConfig );
            iconFontConfig.GlyphOffset = ImVec2( 0, 3 );
            iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 18.0f;
            io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (S32) fontAwesomeData.size(), 16.0f, &iconFontConfig, icons_ranges );
            SystemFonts[(U8) Font::Small] = pSmallFont;

            Fonts::GetDecompressedFontData( Fonts::PixelCarnage::GetFontCompressedData(), fontData );
            fontConfig.GlyphOffset = ImVec2( 0, -1 );
            ImFont* pMediumFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (S32) fontData.size(), 16, &fontConfig );
            iconFontConfig.GlyphOffset = ImVec2( 0, -3 );
            iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 20.0f;
            io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (S32) fontAwesomeData.size(), 18.0f, &iconFontConfig, icons_ranges );
            SystemFonts[(U8) Font::Medium] = pMediumFont;

            Fonts::Roboto::GetFontCompressedData( fontCompressedData );
            Fonts::GetDecompressedFontData( fontCompressedData.data(), fontData );
            fontConfig.GlyphOffset = ImVec2( 0, 0 );
            ImFont* pLargeFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (S32) fontData.size(), 24, &fontConfig );
            iconFontConfig.GlyphOffset = ImVec2( 0, 1 );
            iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 26.0f;
            io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (S32) fontAwesomeData.size(), 24.0f, &iconFontConfig, icons_ranges );
            SystemFonts[(U8) Font::Large] = pLargeFont;
        }

        void SystemBase::InitializeStyle()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_Text] = ImVec4( 0.90f, 0.90f, 0.90f, 1.00f );
            style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
            style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
            style.Colors[ImGuiCol_ChildBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
            style.Colors[ImGuiCol_PopupBg] = ImVec4( 0.05f, 0.05f, 0.10f, 0.90f );
            style.Colors[ImGuiCol_Border] = ImVec4( 0.70f, 0.70f, 0.70f, 0.65f );
            style.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
            style.Colors[ImGuiCol_FrameBg] = ImVec4( 0.44f, 0.44f, 0.44f, 1.00f );
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.47f, 0.47f, 0.47f, 0.40f );
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
            style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.33f, 0.33f, 0.33f, 1.00f );
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.37f, 0.37f, 0.37f, 1.00f );
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.33f, 0.33f, 0.33f, 1.00f );
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.34f, 0.34f, 0.34f, 1.00f );
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.69f, 0.69f, 0.69f, 1.00f );
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.65f, 0.65f, 0.65f, 1.00f );
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.59f, 0.59f, 0.59f, 1.00f );
            style.Colors[ImGuiCol_CheckMark] = ImColor( 0, 200, 0, 255 );
            style.Colors[ImGuiCol_SliderGrab] = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );
            style.Colors[ImGuiCol_SliderGrabActive] = ImColor( 0, 200, 0, 255 );
            style.Colors[ImGuiCol_Button] = ImColor( 112, 112, 112, 255 );
            style.Colors[ImGuiCol_ButtonHovered] = ImColor( 135, 135, 135, 255 );
            style.Colors[ImGuiCol_ButtonActive] = ImColor( 170, 170, 170, 255 );
            style.Colors[ImGuiCol_Header] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.45f, 0.45f, 0.45f, 1.00f );
            style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.45f, 0.45f, 0.45f, 1.00f );
            style.Colors[ImGuiCol_Separator] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.70f, 0.60f, 0.60f, 1.00f );
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4( 0.90f, 0.70f, 0.70f, 1.00f );
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.90f );
            style.Colors[ImGuiCol_Tab] = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );
            style.Colors[ImGuiCol_TabHovered] = ImColor( 30, 185, 0, 255 );
            style.Colors[ImGuiCol_TabActive] = ImColor( 30, 160, 0, 255 );
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
            style.Colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
            style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );

            style.Colors[ImGuiCol_DragDropTarget] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
            style.Colors[ImGuiCol_NavHighlight] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
            style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
            style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
            style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );

            style.WindowPadding = Float2( 8, 8 );
            style.WindowRounding = 0;
            style.WindowBorderSize = 0;
            style.ChildRounding = 0;
            style.FramePadding = Float2( 3, 3 );
            style.FrameRounding = 0;
            style.ItemSpacing = Float2( 4, 4 );
            style.ScrollbarRounding = 0;
            style.GrabRounding = 0;
            style.GrabMinSize = 8;
        }
    }
}