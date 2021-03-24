#include "ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    constexpr auto ColorFromBytes = [] ( uint8_t r, uint8_t g, uint8_t b )
    {
        return ImVec4( (float) r / 255.0f, (float) g / 255.0f, (float) b / 255.0f, 1.0f );
    };

    //-------------------------------------------------------------------------

    ImVec4 const Theme::s_backgroundColorLight = ColorFromBytes( 90, 90, 95 );
    ImVec4 const Theme::s_backgroundColorMedium = ColorFromBytes( 82, 82, 85 );
    ImVec4 const Theme::s_backgroundColorDark = ColorFromBytes( 37, 37, 38 );
    ImVec4 const Theme::s_panelColor = ColorFromBytes( 51, 51, 55 );
    ImVec4 const Theme::s_accentColor = ColorFromBytes( 29, 151, 236 );
    ImVec4 const Theme::s_accentColorDark = ColorFromBytes( 0, 119, 200 );
    ImVec4 const Theme::s_textColor = ColorFromBytes( 255, 255, 255 );
    ImVec4 const Theme::s_textColorDisabled = ColorFromBytes( 151, 151, 151 );
    ImVec4 const Theme::s_borderColor = ColorFromBytes( 78, 78, 78 );

    //-------------------------------------------------------------------------

     void Theme::ApplyTheme()
     {
         //-------------------------------------------------------------------------
         // Colors
         //-------------------------------------------------------------------------

         ImVec4* colors = ImGui::GetStyle().Colors;
         colors[ImGuiCol_Text] = s_textColor;
         colors[ImGuiCol_TextDisabled] = s_textColorDisabled;
         colors[ImGuiCol_TextSelectedBg] = s_accentColorDark;
         colors[ImGuiCol_WindowBg] = s_backgroundColorDark;
         colors[ImGuiCol_ChildBg] = s_backgroundColorDark;
         colors[ImGuiCol_PopupBg] = s_backgroundColorDark;
         colors[ImGuiCol_Border] = s_borderColor;
         colors[ImGuiCol_BorderShadow] = s_borderColor;
         colors[ImGuiCol_FrameBg] = s_panelColor;
         colors[ImGuiCol_FrameBgHovered] = s_accentColor;
         colors[ImGuiCol_FrameBgActive] = s_accentColorDark;
         colors[ImGuiCol_TitleBg] = s_backgroundColorDark;
         colors[ImGuiCol_TitleBgActive] = s_backgroundColorDark;
         colors[ImGuiCol_TitleBgCollapsed] = s_backgroundColorDark;
         colors[ImGuiCol_MenuBarBg] = s_panelColor;
         colors[ImGuiCol_ScrollbarBg] = s_panelColor;
         colors[ImGuiCol_ScrollbarGrab] = s_backgroundColorMedium;
         colors[ImGuiCol_ScrollbarGrabHovered] = s_backgroundColorLight;
         colors[ImGuiCol_ScrollbarGrabActive] = s_backgroundColorLight;
         colors[ImGuiCol_CheckMark] = s_accentColorDark;
         colors[ImGuiCol_SliderGrab] = s_accentColor;
         colors[ImGuiCol_SliderGrabActive] = s_accentColorDark;
         colors[ImGuiCol_Button] = s_panelColor;
         colors[ImGuiCol_ButtonHovered] = s_accentColor;
         colors[ImGuiCol_ButtonActive] = s_accentColor;
         colors[ImGuiCol_Header] = s_panelColor;
         colors[ImGuiCol_HeaderHovered] = s_accentColor;
         colors[ImGuiCol_HeaderActive] = s_accentColorDark;
         colors[ImGuiCol_Separator] = s_borderColor;
         colors[ImGuiCol_SeparatorHovered] = s_borderColor;
         colors[ImGuiCol_SeparatorActive] = s_borderColor;
         colors[ImGuiCol_ResizeGrip] = s_backgroundColorDark;
         colors[ImGuiCol_ResizeGripHovered] = s_panelColor;
         colors[ImGuiCol_ResizeGripActive] = s_backgroundColorMedium;
         colors[ImGuiCol_PlotLines] = s_accentColorDark;
         colors[ImGuiCol_PlotLinesHovered] = s_accentColor;
         colors[ImGuiCol_PlotHistogram] = s_accentColorDark;
         colors[ImGuiCol_PlotHistogramHovered] = s_accentColor;
         colors[ImGuiCol_DragDropTarget] = s_backgroundColorDark;
         colors[ImGuiCol_NavHighlight] = s_backgroundColorDark;
         colors[ImGuiCol_DockingPreview] = s_accentColorDark;
         colors[ImGuiCol_Tab] = s_backgroundColorDark;
         colors[ImGuiCol_TabActive] = s_accentColorDark;
         colors[ImGuiCol_TabUnfocused] = s_backgroundColorDark;
         colors[ImGuiCol_TabUnfocusedActive] = s_accentColorDark;
         colors[ImGuiCol_TabHovered] = s_accentColor;

         //-------------------------------------------------------------------------
         // Style
         //-------------------------------------------------------------------------

         ImGuiStyle& style = ImGui::GetStyle();

         style.WindowRounding = 0.0f;
         style.ChildRounding = 0.0f;
         style.FrameRounding = 0.0f;
         style.GrabRounding = 0.0f;
         style.PopupRounding = 0.0f;
         style.ScrollbarRounding = 0.0f;
         style.TabRounding = 0.0f;
     }
}