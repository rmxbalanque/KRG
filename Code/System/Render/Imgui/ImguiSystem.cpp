#include "ImguiSystem.h"

#if KRG_DEVELOPMENT_TOOLS
#include "ImguiFont.h"
#include "ImguiTheme.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_Roboto.h"
#include "System/Render/Fonts/FontData_FontAwesome4.h"

#include "misc/freetype/imgui_freetype.h"

//-------------------------------------------------------------------------

#if _WIN32
#include "Platform/ImguiPlatform_Win32.h"
#endif

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    bool ImguiSystem::Initialize( String const& iniFilename, Render::RenderDevice* pRenderDevice, bool enableViewports )
    {
        ImGui::CreateContext();

        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        if ( enableViewports )
        {
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        // Set render device in the user data
        KRG_ASSERT( pRenderDevice != nullptr );
        io.BackendRendererUserData = pRenderDevice;

        //-------------------------------------------------------------------------

        m_iniFilename = iniFilename;
        if ( !m_iniFilename.empty() )
        {
            io.IniFilename = m_iniFilename.c_str();
        }

        //-------------------------------------------------------------------------

        Platform::InitializePlatform();
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

        Platform::ShutdownPlatform();
        ImGui::DestroyContext();
    }

    void ImguiSystem::InitializeFonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Decompress fonts
        //-------------------------------------------------------------------------

        TVector<Byte> fontData, boldFontData;
        Fonts::GetDecompressedFontData( Fonts::Roboto::Regular::GetData(), fontData );
        Fonts::GetDecompressedFontData( Fonts::Roboto::Bold::GetData(), boldFontData );

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

        ImFontConfig iconFontConfig;
        iconFontConfig.FontDataOwnedByAtlas = false;
        iconFontConfig.MergeMode = true;
        iconFontConfig.OversampleH = 3;
        iconFontConfig.OversampleV = 1;
        iconFontConfig.RasterizerMultiply = 1.5f;

        auto CreateFont = [&] ( TVector<Byte>& fontData, float fontSize, Font fontID, char const* pName )
        {
            Printf( fontConfig.Name, 40, pName );
            ImFont* pFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), fontSize, &fontConfig );
            SystemFonts::s_fonts[(uint8) fontID] = pFont;

            iconFontConfig.GlyphOffset = ImVec2( 0, 0 );
            iconFontConfig.GlyphMinAdvanceX = fontSize;
            iconFontConfig.GlyphMaxAdvanceX = fontSize;
            io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), fontSize, &iconFontConfig, icons_ranges );
        };

        CreateFont( fontData, 12, Font::Tiny, "Tiny" );
        CreateFont( boldFontData, 12, Font::TinyBold, "Tiny Bold" );

        CreateFont( fontData, 14, Font::Small, "Small" );
        CreateFont( boldFontData, 14, Font::SmallBold, "Small Bold" );

        CreateFont( fontData, 16, Font::Medium, "Medium" );
        CreateFont( boldFontData, 16, Font::MediumBold, "Medium Bold" );

        CreateFont( fontData, 20, Font::Large, "Large" );
        CreateFont( boldFontData, 20, Font::LargeBold, "Large Bold" );

        CreateFont( fontData, 48, Font::Huge, "Huge" );
        CreateFont( boldFontData, 48, Font::HugeBold, "Huge Bold" );

        // Build font atlas
        //-------------------------------------------------------------------------

        io.Fonts->Build();
        KRG_ASSERT( io.Fonts->IsBuilt() );

        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Tiny]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::TinyBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Small]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::SmallBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Medium]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::MediumBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Large]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::LargeBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Huge]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::HugeBold]->IsLoaded() );
        #endif

        io.FontDefault = SystemFonts::s_fonts[(uint8) Font::Small];
    }

    //-------------------------------------------------------------------------

    void ImguiSystem::StartFrame( float deltaTime )
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = deltaTime;

        Platform::UpdateDisplayInformation();
        Platform::UpdateMouseInformation();
        ImGui::NewFrame();
    }

    void ImguiSystem::EndFrame()
    {
        ImGui::EndFrame();
    }
}
#endif