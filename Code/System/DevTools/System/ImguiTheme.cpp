#include "ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    constexpr auto ColorFromBytes = [] ( uint8_t r, uint8_t g, uint8_t b )
    {
        return ImVec4( (float) r / 255.0f, (float) g / 255.0f, (float) b / 255.0f, 1.0f );
    };

    //-------------------------------------------------------------------------

    ImVec4 const Theme::s_backgroundColorLight = ColorFromBytes( 75, 75, 75 );
    ImVec4 const Theme::s_backgroundColorSemiLight = ColorFromBytes( 60, 60, 60);
    ImVec4 const Theme::s_backgroundColorMedium = ColorFromBytes( 40, 40, 40 );
    ImVec4 const Theme::s_backgroundColorSemiDark = ColorFromBytes( 25, 25, 25 );
    ImVec4 const Theme::s_backgroundColorDark = ColorFromBytes( 15, 15, 15 );

    ImVec4 const Theme::s_itemColorLight = ColorFromBytes( 128, 128, 128 );
    ImVec4 const Theme::s_itemColorSemiLight = ColorFromBytes( 105, 105, 105 );
    ImVec4 const Theme::s_itemColorMedium = ColorFromBytes( 87, 87, 87 );
    ImVec4 const Theme::s_itemColorSemiDark = ColorFromBytes( 58, 58, 58 );
    ImVec4 const Theme::s_itemColorDark = ColorFromBytes( 46, 46, 46 );

    ImVec4 const Theme::s_textColor = ColorFromBytes( 255, 255, 255 );
    ImVec4 const Theme::s_textColorDisabled = ColorFromBytes( 130, 130, 130 );

    //-------------------------------------------------------------------------

    void Theme::ApplyTheme()
    {
        //-------------------------------------------------------------------------
        // Colors
        //-------------------------------------------------------------------------

        ImVec4* colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_Text] = s_textColor;
        colors[ImGuiCol_TextDisabled] = s_textColorDisabled;
        colors[ImGuiCol_TextSelectedBg] = s_backgroundColorMedium;

        colors[ImGuiCol_TitleBg] = s_backgroundColorDark;
        colors[ImGuiCol_TitleBgActive] = s_backgroundColorDark;
        colors[ImGuiCol_TitleBgCollapsed] = s_backgroundColorDark;

        colors[ImGuiCol_WindowBg] = s_backgroundColorMedium;
        colors[ImGuiCol_ChildBg] = s_backgroundColorMedium;
        colors[ImGuiCol_PopupBg] = s_backgroundColorSemiDark;
        colors[ImGuiCol_MenuBarBg] = s_backgroundColorDark;

        colors[ImGuiCol_Border] = s_backgroundColorDark;
        colors[ImGuiCol_BorderShadow] = s_backgroundColorDark;

        colors[ImGuiCol_FrameBg] = s_backgroundColorSemiDark;
        colors[ImGuiCol_FrameBgHovered] = s_backgroundColorSemiLight;
        colors[ImGuiCol_FrameBgActive] = s_backgroundColorSemiLight;

        colors[ImGuiCol_Tab] = s_backgroundColorDark;
        colors[ImGuiCol_TabActive] = s_backgroundColorMedium;
        colors[ImGuiCol_TabHovered] = s_backgroundColorSemiLight;
        colors[ImGuiCol_TabUnfocused] = s_backgroundColorDark;
        colors[ImGuiCol_TabUnfocusedActive] = s_backgroundColorMedium;

        colors[ImGuiCol_Header] = s_backgroundColorSemiLight;
        colors[ImGuiCol_HeaderHovered] = s_backgroundColorLight;
        colors[ImGuiCol_HeaderActive] = s_backgroundColorSemiLight;

        colors[ImGuiCol_Separator] = s_backgroundColorDark;
        colors[ImGuiCol_SeparatorHovered] = s_backgroundColorSemiLight;
        colors[ImGuiCol_SeparatorActive] = s_backgroundColorSemiLight;

        colors[ImGuiCol_NavHighlight] = s_backgroundColorDark;
        colors[ImGuiCol_DockingPreview] = s_itemColorMedium;

        colors[ImGuiCol_ScrollbarBg] = s_backgroundColorMedium;
        colors[ImGuiCol_ScrollbarGrab] = s_itemColorMedium;
        colors[ImGuiCol_ScrollbarGrabHovered] = s_itemColorSemiLight;
        colors[ImGuiCol_ScrollbarGrabActive] = s_itemColorSemiLight;

        colors[ImGuiCol_Button] = s_itemColorSemiDark;
        colors[ImGuiCol_ButtonHovered] = s_itemColorMedium;
        colors[ImGuiCol_ButtonActive] = s_itemColorMedium;

        colors[ImGuiCol_CheckMark] = s_itemColorLight;
        colors[ImGuiCol_SliderGrab] = s_itemColorMedium;
        colors[ImGuiCol_SliderGrabActive] = s_itemColorSemiLight;

        colors[ImGuiCol_ResizeGrip] = s_backgroundColorSemiLight;
        colors[ImGuiCol_ResizeGripHovered] = s_backgroundColorLight;
        colors[ImGuiCol_ResizeGripActive] = s_backgroundColorLight;

        colors[ImGuiCol_PlotLines] = s_itemColorSemiLight;
        colors[ImGuiCol_PlotLinesHovered] = s_itemColorLight;
        colors[ImGuiCol_PlotHistogram] = s_itemColorSemiLight;
        colors[ImGuiCol_PlotHistogramHovered] = s_itemColorLight;

        colors[ImGuiCol_DragDropTarget] = s_backgroundColorDark;

        //-------------------------------------------------------------------------
        // Style
        //-------------------------------------------------------------------------

        ImGuiStyle& style = ImGui::GetStyle();

        style.FramePadding = ImVec2( 4, 2 );
        style.WindowPadding = ImVec2( 4, 4 );
        style.ChildBorderSize = 0.0f;
        style.GrabRounding = 2.0f;
        style.GrabMinSize = 6.0f;
        style.WindowRounding = 0.0f;
        style.FrameRounding = 3.0f;
        style.IndentSpacing = 8.0f;
        style.TabRounding = 4.0f;
    }
}