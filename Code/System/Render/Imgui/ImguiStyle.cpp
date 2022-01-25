#include "ImguiStyle.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    ImColor const Style::s_backgroundColorLight( 75, 75, 75 );
    ImColor const Style::s_backgroundColorSemiLight( 60, 60, 60);
    ImColor const Style::s_backgroundColorMedium( 40, 40, 40 );
    ImColor const Style::s_backgroundColorSemiDark( 25, 25, 25 );
    ImColor const Style::s_backgroundColorDark( 15, 15, 15 );

    ImColor const Style::s_itemColorLight( 128, 128, 128 );
    ImColor const Style::s_itemColorSemiLight( 105, 105, 105 );
    ImColor const Style::s_itemColorMedium( 87, 87, 87 );
    ImColor const Style::s_itemColorSemiDark( 58, 58, 58 );
    ImColor const Style::s_itemColorDark( 46, 46, 46 );

    ImColor const Style::s_textColor( 255, 255, 255 );
    ImColor const Style::s_textColorDisabled( 130, 130, 130 );

    ImColor const Style::s_selectionAccentAlt( 0xFF32CD32 );
    ImColor const Style::s_selectionAccent( 0xFF32CD32 );

    ImColor const Style::s_gridBackgroundColor( 40, 40, 40, 200 );
    ImColor const Style::s_gridLineColor( 200, 200, 200, 40 );
    ImColor const Style::s_selectionBoxFillColor( 61, 133, 224, 150 );
    ImColor const Style::s_selectionBoxOutlineColor( 61, 133, 224, 30 );

    //-------------------------------------------------------------------------

    void Style::Apply()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        //-------------------------------------------------------------------------
        // Colors
        //-------------------------------------------------------------------------

        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text] = s_textColor;
        colors[ImGuiCol_TextDisabled] = s_textColorDisabled;
        colors[ImGuiCol_TextSelectedBg] = s_itemColorLight;

        colors[ImGuiCol_TitleBg] = s_backgroundColorDark;
        colors[ImGuiCol_TitleBgActive] = s_backgroundColorDark;
        colors[ImGuiCol_TitleBgCollapsed] = s_backgroundColorDark;

        colors[ImGuiCol_WindowBg] = s_backgroundColorMedium;
        colors[ImGuiCol_ChildBg] = s_backgroundColorMedium;
        colors[ImGuiCol_PopupBg] = s_backgroundColorMedium;
        colors[ImGuiCol_MenuBarBg] = s_backgroundColorSemiDark;

        colors[ImGuiCol_Border] = s_backgroundColorDark;
        colors[ImGuiCol_BorderShadow] = s_backgroundColorDark;

        colors[ImGuiCol_FrameBg] = s_backgroundColorSemiLight;
        colors[ImGuiCol_FrameBgHovered] = s_backgroundColorSemiLight;
        colors[ImGuiCol_FrameBgActive] = s_backgroundColorSemiLight;

        colors[ImGuiCol_Tab] = s_backgroundColorDark;
        colors[ImGuiCol_TabActive] = s_itemColorSemiLight;
        colors[ImGuiCol_TabHovered] = s_itemColorLight;
        colors[ImGuiCol_TabUnfocused] = s_backgroundColorDark;
        colors[ImGuiCol_TabUnfocusedActive] = s_backgroundColorMedium;

        colors[ImGuiCol_Header] = s_backgroundColorSemiLight;
        colors[ImGuiCol_HeaderHovered] = s_backgroundColorLight;
        colors[ImGuiCol_HeaderActive] = s_backgroundColorSemiLight;

        colors[ImGuiCol_Separator] = s_itemColorMedium;
        colors[ImGuiCol_SeparatorHovered] = s_itemColorSemiLight;
        colors[ImGuiCol_SeparatorActive] = s_itemColorLight;

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

        style.FramePadding = ImVec2( 4, 4 );
        style.WindowPadding = ImVec2( 4, 4 );
        style.ChildBorderSize = 0.0f;
        style.GrabRounding = 2.0f;
        style.GrabMinSize = 6.0f;
        style.WindowRounding = 0.0f;
        style.FrameRounding = 3.0f;
        style.IndentSpacing = 8.0f;
        style.ItemSpacing = ImVec2( 4, 4 );
        style.TabRounding = 4.0f;
    }
}