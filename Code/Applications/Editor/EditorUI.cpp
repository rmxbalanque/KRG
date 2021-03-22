#include "EditorUI.h"

#include "System/Core/Update/UpdateStage.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Render/RenderViewportSystem.h"
#include "System/Imgui/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    void EditorUI::Initialize( SettingsRegistry const& settingsRegistry )
    {
    }

    void EditorUI::Shutdown()
    {

    }

    void EditorUI::Update( UpdateContext const& context, Render::ViewportSystem& viewportSystem )
    {
        if ( !viewportSystem.HasActiveViewports() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                DrawDockSpace();
                
                ImGui::ShowDemoWindow();
            }
            break;

            case UpdateStage::FrameEnd:
            {
            }
            break;
        }

        //-------------------------------------------------------------------------

        viewportSystem.ResizePrimaryViewport( m_viewportRect );
    }

    void EditorUI::DrawDockSpace()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

        ImGui::SetNextWindowBgAlpha( 0.0f );
        ImGui::Begin( "DockSpace", nullptr, windowFlags );
        {
            ImGuiDockNodeFlags const dockSpaceFlags =  ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;
            m_mainDockspaceID = ImGui::GetID( "EditorDockSpace" );
            ImGui::DockSpace( m_mainDockspaceID, ImVec2( 0.0f, 0.0f ), dockSpaceFlags );
        }
        ImGui::PopStyleVar( 3 );
        ImGui::End();

        //-------------------------------------------------------------------------

        auto pCentralNode = ImGui::DockBuilderGetCentralNode( m_mainDockspaceID );
        m_viewportRect = Math::Rectangle( pCentralNode->Pos, pCentralNode->Size );
    }
}
#endif