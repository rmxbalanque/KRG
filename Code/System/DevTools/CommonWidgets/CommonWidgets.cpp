#include "CommonWidgets.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    void DrawOrientationGuide( Math::Viewport const& viewport, ImVec2 const& margin )
    {
        static float const windowPadding = 4.0f;
        static float const windowRounding = 2.0f;
        static float const windowDimension = 55.0f;
        static float const axisHeadRadius = 3.0f;
        static float const axisHalfLength = ( windowDimension / 2 ) - axisHeadRadius - 4.0f;
        static float const worldRenderDistanceZ = 5.0f;
        static float const axisThickness = 2.0f;

        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        static bool alwaysOpen = true;

        Float2 const viewportBottomLeft = viewport.GetTopLeftPosition() + viewport.GetSize();

        Float2 const windowDimensions( windowDimension, windowDimension );
        Float2 const windowPosition = ImVec2( viewport.GetTopLeftPosition().m_x + margin.x, viewportBottomLeft.m_y - windowDimensions.m_y - margin.y );
        Float2 const windowCenter = Float2( windowPosition.m_x + windowDimensions.m_x / 2, windowPosition.m_y + windowDimensions.m_y / 2 );

        ImGui::SetNextWindowSize( windowDimensions );
        ImGui::SetNextWindowPos( windowPosition );
        ImGui::SetNextWindowBgAlpha( 0.0f );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, ImVec2( 0.0f, 0.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( windowPadding, windowPadding ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, windowRounding );
        uint32 const flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
        if ( ImGui::Begin( "DebugUI_OrientationGuide", &alwaysOpen, flags ) )
        {
            // Project world space axis positions to screen space
            //-------------------------------------------------------------------------

            Vector const& originWS = viewport.GetViewOrigin() + viewport.GetViewForwardDirection() * worldRenderDistanceZ;
            Vector const& worldAxisX = ( Vector::UnitX );
            Vector const& worldAxisY = ( Vector::UnitY );
            Vector const& worldAxisZ = ( Vector::UnitZ );

            Vector const& worldAxisForwardPointX = ( originWS + worldAxisX );
            Vector const& worldAxisForwardPointY = ( originWS + worldAxisY );
            Vector const& worldAxisForwardPointZ = ( originWS + worldAxisZ );
            Vector const& worldAxisBackwardPointX = ( originWS - worldAxisX );
            Vector const& worldAxisBackwardPointY = ( originWS - worldAxisY );
            Vector const& worldAxisBackwardPointZ = ( originWS - worldAxisZ );

            Vector const axisStartPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointX ) );
            Vector const axisStartPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointY ) );
            Vector const axisStartPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointZ ) );
            Vector const axisEndPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointX ) );
            Vector const axisEndPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointY ) );
            Vector const axisEndPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointZ ) );

            // Calculate screen space axis lengths
            //-------------------------------------------------------------------------

            float const axisLengthX = axisStartPointX.GetDistance2( axisEndPointX );
            float const axisLengthY = axisStartPointY.GetDistance2( axisEndPointY );
            float const axisLengthZ = axisStartPointZ.GetDistance2( axisEndPointZ );
            float const maxAxisLength = Math::Max( axisLengthX, Math::Max( axisLengthY, axisLengthZ ) );

            static float const axisHalfLengthSS = axisHalfLength;
            float const axisScaleX = ( axisLengthX / maxAxisLength ) * axisHalfLengthSS;
            float const axisScaleY = ( axisLengthY / maxAxisLength ) * axisHalfLengthSS;
            float const axisScaleZ = ( axisLengthZ / maxAxisLength ) * axisHalfLengthSS;

            // Calculate screen space axis directions
            Vector const origin = viewport.WorldSpaceToScreenSpace( originWS );
            Vector const axisDirX = ( axisEndPointX - origin ).GetNormalized2();
            Vector const axisDirY = ( axisEndPointY - origin ).GetNormalized2();
            Vector const axisDirZ = ( axisEndPointZ - origin ).GetNormalized2();

            // Sort SS axis and draw them in the correct order
            //-------------------------------------------------------------------------

            struct AxisDrawRequest { Axis m_axis; bool m_isInForwardDirection; float m_distance; };
            TInlineVector<AxisDrawRequest, 3> drawRequests;

            Plane const NearPlane = viewport.GetViewVolume().GetViewPlane( Math::ViewVolume::PlaneID::Near );

            drawRequests.push_back( { Axis::X, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointX ) } );
            drawRequests.push_back( { Axis::Y, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointY ) } );
            drawRequests.push_back( { Axis::Z, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointZ ) } );
            drawRequests.push_back( { Axis::X, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointX ) } );
            drawRequests.push_back( { Axis::Y, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointY ) } );
            drawRequests.push_back( { Axis::Z, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointZ ) } );

            eastl::sort( drawRequests.begin(), drawRequests.end(), [] ( AxisDrawRequest const& lhs, AxisDrawRequest const& rhs ) { return lhs.m_distance > rhs.m_distance; } );

            //-------------------------------------------------------------------------

            auto pDrawList = ImGui::GetWindowDrawList();
            for ( auto const& request : drawRequests )
            {
                // X
                if ( request.m_axis == Axis::X && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirX * ( axisScaleX - axisHeadRadius + 1.0f ), 0xBB0000FF, axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirX * axisScaleX, axisHeadRadius, 0xBB0000FF );
                }
                else if ( request.m_axis == Axis::X && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirX * axisScaleX, axisHeadRadius, 0x660000FF );
                }
                //Y
                else if ( request.m_axis == Axis::Y && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirY * ( axisScaleY - axisHeadRadius + 1.0f ), 0xBB00FF00, axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirY * axisScaleY, axisHeadRadius, 0xBB00FF00 );
                }
                else if ( request.m_axis == Axis::Y && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirY * axisScaleY, axisHeadRadius, 0x6600FF00 );
                }
                // Z
                else if ( request.m_axis == Axis::Z && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirZ * ( axisScaleZ - axisHeadRadius + 1.0f ), 0xBBFF0000, axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirZ * axisScaleZ, axisHeadRadius, 0xBBFF0000 );
                }
                else if ( request.m_axis == Axis::Z && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirZ * axisScaleZ, axisHeadRadius, 0x66FF0000 );
                }
            }

            ImGui::End();
        }
        ImGui::PopStyleVar( 3 );
    }

    //-------------------------------------------------------------------------

    bool DrawSpinner( const char* pLabel, ImVec4 color, float radius, float thickness )
    {
        static float const numSegments = 30.0f;

        //-------------------------------------------------------------------------

        ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
        if ( pWindow->SkipItems )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        ImGuiStyle const& style = ImGui::GetStyle();
        ImGuiID const id = pWindow->GetID( pLabel );

        ImVec2 const pos = pWindow->DC.CursorPos;
        ImVec2 const size( ( radius ) * 2, ( radius + style.FramePadding.y ) * 2 );

        ImRect const bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
        ImGui::ItemSize( bb, style.FramePadding.y );
        if ( !ImGui::ItemAdd( bb, id ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        pWindow->DrawList->PathClear();

        float const time = (float) ImGui::GetTime();
        float const start = Math::Abs( Math::Sin( time * 1.8f ) * ( numSegments - 5 ) );
        float const min = Math::Pi * 2.0f * ( start ) / numSegments;
        float const max = Math::Pi * 2.0f * ( numSegments - 3 ) / numSegments;

        ImVec2 const center = ImVec2( pos.x + radius, pos.y + radius + style.FramePadding.y );

        for ( float i = 0; i < numSegments; i++ )
        {
            float const a = min + ( i / numSegments ) * ( max - min );
            float const b = a + ( time * 8 );
            pWindow->DrawList->PathLineTo( ImVec2( center.x + Math::Cos( b ) * radius, center.y + Math::Sin( b ) * radius ) );
        }

        pWindow->DrawList->PathStroke( ImGui::ColorConvertFloat4ToU32( color ), false, thickness );

        return true;
    }
}