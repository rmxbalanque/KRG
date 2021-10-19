#include "ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    void VerticalSeparator( ImVec2 const& size, ImColor const& color )
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

    ImVec2 const& GetClosestPointOnRect( ImRect const& rect, ImVec2 const& inPoint )
    {
        ImVec2 const points[4] =
        {
            ImLineClosestPoint( rect.GetTL(), rect.GetTR(), inPoint ),
            ImLineClosestPoint( rect.GetBL(), rect.GetBR(), inPoint ),
            ImLineClosestPoint( rect.GetTL(), rect.GetBL(), inPoint ),
            ImLineClosestPoint( rect.GetTR(), rect.GetBR(), inPoint )
        };

        float distancesSq[4] =
        {
            ImLengthSqr( points[0] - inPoint ),
            ImLengthSqr( points[1] - inPoint ),
            ImLengthSqr( points[2] - inPoint ),
            ImLengthSqr( points[3] - inPoint )
        };

        // Get closest point
        float lowestDistance = FLT_MAX;
        int32 closestPointIdx = -1;
        for ( auto i = 0; i < 4; i++ )
        {
            if ( distancesSq[i] < lowestDistance )
            {
                closestPointIdx = i;
                lowestDistance = distancesSq[i];
            }
        }

        KRG_ASSERT( closestPointIdx >= 0 && closestPointIdx < 4 );
        return points[closestPointIdx];
    }

    void DrawArrow( ImDrawList* pDrawList, ImVec2 const& arrowStart, ImVec2 const& arrowEnd, ImU32 col, float arrowWidth, float arrowHeadWidth )
    {
        KRG_ASSERT( pDrawList != nullptr );

        ImVec2 const direction = Vector( arrowEnd - arrowStart ).GetNormalized2();
        ImVec2 const orthogonalDirection( -direction.y, direction.x );

        ImVec2 const triangleSideOffset = orthogonalDirection * arrowHeadWidth;
        ImVec2 const triBase = arrowEnd - ( direction * arrowHeadWidth );
        ImVec2 const tri1 = triBase - triangleSideOffset;
        ImVec2 const tri2 = triBase + triangleSideOffset;

        pDrawList->AddLine( arrowStart, triBase, col, arrowWidth );
        pDrawList->AddTriangleFilled( arrowEnd, tri1, tri2, col );
    }
}