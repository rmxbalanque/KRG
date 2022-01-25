#include "ImguiSystem.h"

#if KRG_DEVELOPMENT_TOOLS
#include "ImguiFont.h"
#include "ImguiStyle.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_Roboto.h"
#include "System/Render/Fonts/FontData_FontAwesome5.h"

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

        Style::Apply();

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
        Fonts::FontAwesome5::GetFontCompressedData( iconFontCompressedData );
        Fonts::GetDecompressedFontData( iconFontCompressedData.data(), iconFontData );

        // Base font configs
        //-------------------------------------------------------------------------

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        fontConfig.OversampleH = 4;
        fontConfig.OversampleV = 4;

        ImFontConfig iconFontConfig;
        iconFontConfig.FontDataOwnedByAtlas = false;
        iconFontConfig.MergeMode = true;
        iconFontConfig.OversampleH = 1;
        iconFontConfig.OversampleV = 1;
        iconFontConfig.PixelSnapH = true;
        iconFontConfig.RasterizerMultiply = 1.5f;

        auto CreateFont = [&] ( TVector<Byte>& fontData, float fontSize, float iconFontSize, Font fontID, char const* pName, float glyphAdvanceOffset = 3, ImVec2 const& glyphOffset = ImVec2( 0, 0 ) )
        {
            Printf( fontConfig.Name, 40, pName );
            ImFont* pFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), fontSize, &fontConfig );
            SystemFonts::s_fonts[(uint8) fontID] = pFont;

            iconFontConfig.GlyphOffset = glyphOffset;
            iconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMaxAdvanceX = fontSize + glyphAdvanceOffset;
            io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), iconFontSize, &iconFontConfig, icons_ranges );
        };

        CreateFont( fontData, 12, 12, Font::Tiny, "Tiny", 3, ImVec2( -2, 1 ) );
        CreateFont( boldFontData, 13, 12, Font::TinyBold, "Tiny Bold", 3, ImVec2( -2, 1 ) );

        CreateFont( fontData, 15, 15, Font::Small, "Small", 3, ImVec2( -1, 1 ) );
        CreateFont( boldFontData, 15, 15, Font::SmallBold, "Small Bold", 3, ImVec2( -1, 1 ) );

        CreateFont( fontData, 17, 15, Font::Medium, "Medium", 3, ImVec2( -1, 1 ) );
        CreateFont( boldFontData, 17, 15, Font::MediumBold, "Medium Bold", 3, ImVec2( -1, 1 ) );

        CreateFont( fontData, 21, 18, Font::Large, "Large", 3, ImVec2( -2, 1 ) );
        CreateFont( boldFontData, 21, 18, Font::LargeBold, "Large Bold", 3, ImVec2( -2, 1 ) );

        CreateFont( fontData, 36, 32, Font::Huge, "Huge", 3, ImVec2( -2, 2 ) );
        CreateFont( boldFontData, 36, 32, Font::HugeBold, "Huge Bold", 3, ImVec2( -2, 2 ) );

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