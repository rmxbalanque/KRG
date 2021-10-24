#include "Gizmo.h"
#include "imgui.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    void Gizmo::Update( Render::Viewport const& viewport )
    {
        m_manipulationTransform = m_targetTransform;
        if ( m_coordinateSpace == CoordinateSpace::World )
        {
            m_manipulationTransform.SetRotation( Quaternion::Identity );
        }

        // Calculate shared data
        //-------------------------------------------------------------------------

        m_origin_WS = m_manipulationTransform.GetTranslation();
        m_axisDir_WS_X = m_manipulationTransform.GetUnitAxisX();
        m_axisDir_WS_Y = m_manipulationTransform.GetUnitAxisY();
        m_axisDir_WS_Z = m_manipulationTransform.GetUnitAxisZ();

        m_axisEndPoint_WS_X = m_origin_WS + ( m_axisDir_WS_X * 0.01f );
        m_axisEndPoint_WS_Y = m_origin_WS + ( m_axisDir_WS_Y * 0.01f );
        m_axisEndPoint_WS_Z = m_origin_WS + ( m_axisDir_WS_Z * 0.01f );

        m_origin_SS = Vector( viewport.WorldSpaceToScreenSpace( m_origin_WS ) );

        m_axisEndPoint_SS_X = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_X ) );
        m_axisEndPoint_SS_Y = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_Y ) );
        m_axisEndPoint_SS_Z = Vector( viewport.WorldSpaceToScreenSpace( m_axisEndPoint_WS_Z ) );

        m_axisDir_SS_X = ( m_axisEndPoint_SS_X - m_origin_SS ).GetNormalized2();
        m_axisDir_SS_Y = ( m_axisEndPoint_SS_Y - m_origin_SS ).GetNormalized2();
        m_axisDir_SS_Z = ( m_axisEndPoint_SS_Z - m_origin_SS ).GetNormalized2();

        Vector const viewForwardDir_WS = viewport.GetViewForwardDirection();
        Vector const viewRightDir_WS = viewport.GetViewRightDirection();

        m_offsetBetweenViewFwdAndAxis_WS_X = Math::Min( viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_X ), viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_X.GetNegated() ) );
        m_offsetBetweenViewFwdAndAxis_WS_Y = Math::Min( viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_Y ), viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_Y.GetNegated() ) );
        m_offsetBetweenViewFwdAndAxis_WS_Z = Math::Min( viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_Z ), viewForwardDir_WS.GetAngleBetween( m_axisDir_WS_Z.GetNegated() ) );

        // Draw Window
        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();

        const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::SetNextWindowSize( io.DisplaySize );
        ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
        ImGui::SetNextWindowBgAlpha( 0.0f );

        ImGui::PushStyleColor( ImGuiCol_WindowBg, 0 );
        ImGui::PushStyleColor( ImGuiCol_Border, 0 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );

        ImGui::Begin( "Gizmo", NULL, flags );

        //-------------------------------------------------------------------------

        InternalUpdate( viewport );

        //-------------------------------------------------------------------------

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor( 2 );
    }
}