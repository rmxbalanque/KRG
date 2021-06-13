#include "ImguiX.h"

//-------------------------------------------------------------------------

void KRG::ImGuiX::VerticalSeparator( ImVec2 const& size, ImColor const& color )
{
    ImGui::SameLine( 0, 0 );

    //-------------------------------------------------------------------------

    auto const canvasPos = ImGui::GetCursorScreenPos();
    auto const availableRegion = ImGui::GetContentRegionAvail();

    ImVec2 const seperatorSize( size.x <= 0 ? 1 : size.x, size.y <= 0 ? availableRegion.y : size.y );
    ImGui::Dummy( seperatorSize );
    ImGui::SameLine( 0, 0 );

    //-------------------------------------------------------------------------

    ImColor const separatorColor = ( (int) color == 0 ) ? ImColor( ImGuiX::Theme::s_backgroundColorLight ) : ImColor( color );

    float const startPosX = Math::Floor( canvasPos.x + ( seperatorSize.x / 2 ) );
    float const startPosY = canvasPos.y + 1;
    float const endPosY = startPosY + seperatorSize.y - 2;

    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddLine( ImVec2( startPosX, startPosY ), ImVec2( startPosX, endPosY ), separatorColor, 1 );
}