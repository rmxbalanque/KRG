#include "ImguiSystem.h"
#include "ImguiFont.h"
#include "ImguiTheme.h"
#include "System/DevTools/ThirdParty/imgui/misc/freetype/imgui_freetype.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_Segoe.h"
#include "System/Render/Fonts/FontData_SegoeBold.h"
#include "System/Render/Fonts/FontData_FontAwesome4.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    bool ImguiSystem::Initialize( String const& iniFilename )
    {
        ImGui::CreateContext();

        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        //-------------------------------------------------------------------------

        m_iniFilename = iniFilename;
        if ( !m_iniFilename.empty() )
        {
            io.IniFilename = m_iniFilename.c_str();
        }

        //-------------------------------------------------------------------------

        InitializePlatform();
        InitializeFonts();

        //-------------------------------------------------------------------------

        Theme::ApplyTheme();

        return true;
    }

    void ImguiSystem::Shutdown()
    {
        for ( int i = 0; i < (int8) Font::NumFonts; i++ )
        {
            SystemFonts::s_fonts[i] = nullptr;
        }

        ImGui::DestroyContext();
    }

    void ImguiSystem::StartFrame( float deltaTime )
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

    void ImguiSystem::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImguiSystem::InitializeFonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Decompress fonts
        //-------------------------------------------------------------------------

        TVector<Byte> fontCompressedData, fontData;
        Fonts::Segoe::GetFontCompressedData( fontCompressedData );
        Fonts::GetDecompressedFontData( fontCompressedData.data(), fontData );

        TVector<Byte> fontBoldCompressedData, foldBoldData;
        Fonts::SegoeBold::GetFontCompressedData( fontBoldCompressedData );
        Fonts::GetDecompressedFontData( fontBoldCompressedData.data(), foldBoldData );

        TVector<Byte> iconFontCompressedData, iconFontData;
        ImWchar const icons_ranges[] = { KRG_ICON_MIN_FA, KRG_ICON_MAX_FA, 0 };
        Fonts::FontAwesome4::GetFontCompressedData( iconFontCompressedData );
        Fonts::GetDecompressedFontData( iconFontCompressedData.data(), iconFontData );

        // Base font configs
        //-------------------------------------------------------------------------

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        fontConfig.OversampleH = 8;
        fontConfig.OversampleV = 8;
        fontConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;

        ImFontConfig iconFontConfig;
        iconFontConfig.FontDataOwnedByAtlas = false;
        iconFontConfig.MergeMode = true;
        iconFontConfig.OversampleH = 7;
        iconFontConfig.OversampleV = 5;
        iconFontConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;
        iconFontConfig.RasterizerMultiply = 1.5f;

        // Extra Small Font
        //-------------------------------------------------------------------------

        fontConfig.GlyphExtraSpacing = ImVec2( 1, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -1 );
        ImFont* pExtraSmallFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 12, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::ExtraSmall] = pExtraSmallFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 10.0f, &iconFontConfig, icons_ranges );

        // Small Font
        //-------------------------------------------------------------------------

        fontConfig.GlyphExtraSpacing = ImVec2( 1, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -1 );
        ImFont* pSmallFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 14, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Small] = pSmallFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 14.0f, &iconFontConfig, icons_ranges );

        fontConfig.GlyphExtraSpacing = ImVec2( 1, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -1 );
        ImFont* pSmallBoldFont = io.Fonts->AddFontFromMemoryTTF( foldBoldData.data(), (int32) foldBoldData.size(), 14, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::SmallBold] = pSmallBoldFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 14.0f, &iconFontConfig, icons_ranges );

        // Medium Font
        //-------------------------------------------------------------------------

        fontConfig.GlyphExtraSpacing = ImVec2( 0, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -1 );
        ImFont* pMediumFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 16, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Medium] = pMediumFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 14.0f, &iconFontConfig, icons_ranges );

        fontConfig.GlyphExtraSpacing = ImVec2( 0, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -1 );
        ImFont* pMediumBoldFont = io.Fonts->AddFontFromMemoryTTF( foldBoldData.data(), (int32) foldBoldData.size(), 16, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::MediumBold] = pMediumBoldFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 14.0f, &iconFontConfig, icons_ranges );

        // Large Font
        //-------------------------------------------------------------------------

        fontConfig.GlyphExtraSpacing = ImVec2( 0, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, -2 );
        ImFont* pLargeFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 18, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::Large] = pLargeFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, -1 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 16.0f, &iconFontConfig, icons_ranges );

        // Extra Large Font
        //-------------------------------------------------------------------------

        fontConfig.GlyphExtraSpacing = ImVec2( 0, 0 );
        fontConfig.GlyphOffset = ImVec2( 0, 0 );
        ImFont* pExtraLargeFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), 20, &fontConfig );
        SystemFonts::s_fonts[(uint8) Font::ExtraLarge] = pExtraLargeFont;

        iconFontConfig.GlyphOffset = ImVec2( 0, 1 );
        io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), 18.0f, &iconFontConfig, icons_ranges );

        // Build font atlas
        //-------------------------------------------------------------------------

        io.Fonts->Build();
        io.FontDefault = pSmallFont;

        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( pExtraSmallFont->IsLoaded() );
        KRG_ASSERT( pSmallFont->IsLoaded() );
        KRG_ASSERT( pMediumFont->IsLoaded() );
        KRG_ASSERT( pLargeFont->IsLoaded() );
        KRG_ASSERT( pExtraLargeFont->IsLoaded() );
        #endif
    }
}