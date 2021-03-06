#include "ImguiSystemBase.h"
#include "ImguiFont.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_ProggyTiny.h"
#include "System/Render/Fonts/FontData_ProggyClean.h"
#include "System/Render/Fonts/FontData_Roboto.h"
#include "System/Render/Fonts/FontData_FontAwesome4.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    bool ImguiSystemBase::Initialize()
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

    void ImguiSystemBase::Shutdown()
    {
        for ( int i = 0; i < (int8) Font::NumFonts; i++ )
        {
            SystemFonts::s_fonts[i] = nullptr;
        }

        ImGui::DestroyContext();
    }

    void ImguiSystemBase::StartFrame( float deltaTime )
    {
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

        //-------------------------------------------------------------------------

        ImGui::NewFrame();
    }

    void ImguiSystemBase::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImguiSystemBase::InitializeFonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        //-------------------------------------------------------------------------

        TVector<Byte> fontAwesomeCompressedData, fontAwesomeData;
        Fonts::FontAwesome4::GetFontCompressedData( fontAwesomeCompressedData );
        Fonts::GetDecompressedFontData( fontAwesomeCompressedData.data(), fontAwesomeData );
        ImWchar const icons_ranges[] = { KRG_ICON_MIN_FA, KRG_ICON_MAX_FA, 0 };

        TVector<Byte> fontData;
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
        ImFont* pSmallFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 10, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Small] = pSmallFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 12.0f;
        io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (int32) fontAwesomeData.size(), 10.0f, &iconFontConfig, icons_ranges );

        //-------------------------------------------------------------------------

        Fonts::GetDecompressedFontData( Fonts::ProggyClean::GetFontCompressedData(), fontData );
        fontConfig.GlyphOffset = ImVec2( 0, 0 );
        ImFont* pMediumFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 13, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Medium] = pMediumFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 13.0f;
        io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (int32) fontAwesomeData.size(), 11.0f, &iconFontConfig, icons_ranges );

        //-------------------------------------------------------------------------

        TVector<Byte> fontCompressedData;
        Fonts::Roboto::GetFontCompressedData( fontCompressedData );
        Fonts::GetDecompressedFontData( fontCompressedData.data(), fontData );

        fontConfig.GlyphOffset = ImVec2( 0, 0 );
        ImFont* pLargeFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 18, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Large] = pLargeFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 18.0f;
        io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (int32) fontAwesomeData.size(), 16.0f, &iconFontConfig, icons_ranges );

        //-------------------------------------------------------------------------

        fontConfig.GlyphOffset = ImVec2( 0, 0 );
        ImFont* pExtraLargeFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 24, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::ExtraLarge] = pExtraLargeFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = 24.0f;
        io.Fonts->AddFontFromMemoryTTF( fontAwesomeData.data(), (int32) fontAwesomeData.size(), 20.0f, &iconFontConfig, icons_ranges );

        // Build font atlas
        //-------------------------------------------------------------------------

        io.Fonts->Build();

        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( pSmallFont->IsLoaded() );
        KRG_ASSERT( pMediumFont->IsLoaded() );
        KRG_ASSERT( pLargeFont->IsLoaded() );
        KRG_ASSERT( pExtraLargeFont->IsLoaded() );
        #endif
    }

    void ImguiSystemBase::InitializeStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4( 0.90f, 0.90f, 0.90f, 1.00f );
        colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
        colors[ImGuiCol_WindowBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
        colors[ImGuiCol_ChildBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
        colors[ImGuiCol_PopupBg] = ImVec4( 0.05f, 0.05f, 0.10f, 0.90f );
        colors[ImGuiCol_Border] = ImVec4( 0.70f, 0.70f, 0.70f, 0.65f );
        colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
        colors[ImGuiCol_FrameBg] = ImVec4( 0.44f, 0.44f, 0.44f, 1.00f );
        colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.47f, 0.47f, 0.47f, 0.40f );
        colors[ImGuiCol_FrameBgActive] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
        colors[ImGuiCol_TitleBg] = ImVec4( 0.33f, 0.33f, 0.33f, 1.00f );
        colors[ImGuiCol_TitleBgActive] = ImVec4( 0.37f, 0.37f, 0.37f, 1.00f );
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.33f, 0.33f, 0.33f, 1.00f );
        colors[ImGuiCol_MenuBarBg] = ImVec4( 0.34f, 0.34f, 0.34f, 1.00f );
        colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
        colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.69f, 0.69f, 0.69f, 1.00f );
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.65f, 0.65f, 0.65f, 1.00f );
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.59f, 0.59f, 0.59f, 1.00f );
        colors[ImGuiCol_CheckMark] = ImVec4( 0.88f, 0.56f, 1.00f, 1.00f );
        colors[ImGuiCol_SliderGrab] = ImVec4( 0.28f, 0.28f, 0.28f, 1.00f );
        colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.36f, 0.36f, 0.36f, 1.00f );
        colors[ImGuiCol_Button] = ImVec4( 0.44f, 0.44f, 0.44f, 1.00f );
        colors[ImGuiCol_ButtonHovered] = ImVec4( 0.53f, 0.53f, 0.53f, 1.00f );
        colors[ImGuiCol_ButtonActive] = ImVec4( 0.67f, 0.67f, 0.67f, 1.00f );
        colors[ImGuiCol_Header] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
        colors[ImGuiCol_HeaderHovered] = ImVec4( 0.45f, 0.45f, 0.45f, 1.00f );
        colors[ImGuiCol_HeaderActive] = ImVec4( 0.45f, 0.45f, 0.45f, 1.00f );
        colors[ImGuiCol_Separator] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
        colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.89f, 0.90f, 0.92f, 1.00f );
        colors[ImGuiCol_SeparatorActive] = ImVec4( 0.90f, 0.90f, 0.92f, 1.00f );
        colors[ImGuiCol_ResizeGrip] = ImVec4( 0.89f, 0.90f, 0.92f, 1.00f );
        colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.89f, 0.90f, 0.92f, 1.00f );
        colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.89f, 0.90f, 0.92f, 1.00f );
        colors[ImGuiCol_Tab] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
        colors[ImGuiCol_TabHovered] = ImVec4( 0.63f, 0.44f, 0.70f, 1.00f );
        colors[ImGuiCol_TabActive] = ImVec4( 0.60f, 0.29f, 0.71f, 1.00f );
        colors[ImGuiCol_TabUnfocused] = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.51f, 0.28f, 0.60f, 1.00f );
        colors[ImGuiCol_DockingPreview] = ImVec4( 0.71f, 0.72f, 0.73f, 0.70f );
        colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
        colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
        colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
        colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
        colors[ImGuiCol_TableHeaderBg] = ImVec4( 0.19f, 0.19f, 0.20f, 1.00f );
        colors[ImGuiCol_TableBorderStrong] = ImVec4( 0.31f, 0.31f, 0.35f, 1.00f );
        colors[ImGuiCol_TableBorderLight] = ImVec4( 0.23f, 0.23f, 0.25f, 1.00f );
        colors[ImGuiCol_TableRowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
        colors[ImGuiCol_TableRowBgAlt] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
        colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
        colors[ImGuiCol_DragDropTarget] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
        colors[ImGuiCol_NavHighlight] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 0.16f, 0.16f, 0.16f, 0.71f );
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );

        //-------------------------------------------------------------------------

        style.WindowPadding = Float2( 6, 6 );
        style.FramePadding = Float2( 3, 3 );
        style.ItemSpacing = Float2( 4, 4 );
        style.CellPadding = Float2( 4, 2 );

        style.WindowBorderSize = 1;

        style.WindowRounding = 0;
        style.ChildRounding = 0;
        style.FrameRounding = 0;
        style.ScrollbarRounding = 0;
        style.GrabRounding = 0;
        style.TabRounding = 2;

        style.GrabMinSize = 8;
    }
}