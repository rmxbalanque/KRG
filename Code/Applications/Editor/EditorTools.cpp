#include "EditorTools.h"

#include "System/Core/Update/UpdateStage.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Input/InputSystem.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"
#include "System/DevTools/CommonWidgets/CommonWidgets.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    void EditorTools::Initialize( SettingsRegistry const& settingsRegistry )
    {
    }

    void EditorTools::Shutdown()
    {

    }

    void EditorTools::Update( UpdateContext const& context, Render::ViewportManager& viewportSystem )
    {
        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                DrawMainMenu( context );
                DrawDockSpaceAndViewport( context, viewportSystem );
                ImGui::ShowDemoWindow();
            }
            break;

            case UpdateStage::FrameEnd:
            {
            }
            break;
        }

        //-------------------------------------------------------------------------

        // Disable input when we the mouse is not within the editor viewport
        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );
        pInputSystem->SetEnabled( m_mouseWithinEditorViewport );
    }

    void EditorTools::DrawMainMenu( UpdateContext const& context )
    {
        if ( ImGui::BeginMainMenuBar() )
        {
            //-------------------------------------------------------------------------
            // Draw Menu Items
            //-------------------------------------------------------------------------

            if ( ImGui::BeginMenu( "File" ) )
            {
                ImGui::EndMenu();
            }

            //-------------------------------------------------------------------------
            // Draw Performance Stats
            //-------------------------------------------------------------------------

            ImGuiViewport const* viewport = ImGui::GetMainViewport();
            ImGui::SameLine( viewport->WorkSize.x - 165 );

            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
            ImGui::Text( "FPS: %3.0f | Mem: %.2fMB", currentFPS,  allocatedMemory );

            ImGui::EndMainMenuBar();
        }
    }

    void EditorTools::DrawDockSpaceAndViewport( UpdateContext const& context, Render::ViewportManager& viewportSystem )
    {
        ImGuiID dockspaceID = ImGui::GetID( "EditorDockSpace" );

        //-------------------------------------------------------------------------
        // Create Dock Space Window
        //-------------------------------------------------------------------------

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

        ImGui::SetNextWindowBgAlpha( 0.0f );
        ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        {
            // Always ensure that the viewport is docked into the central node
            if ( ImGuiDockNode* pCentralNode = ImGui::DockBuilderGetCentralNode( dockspaceID ) )
            {
                pCentralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
                ImGui::DockBuilderDockWindow( "EditorViewport", pCentralNode->ID );
            }

            // Create the actual dock space
            ImGuiDockNodeFlags const dockSpaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode;
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, dockSpaceFlags );
        }
        ImGui::PopStyleVar( 3 );
        ImGui::End();

        //-------------------------------------------------------------------------
        // Create viewport window
        //-------------------------------------------------------------------------

        windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse ;

        ImGui::SetNextWindowBgAlpha( 0.0f );
        if ( ImGui::Begin( "EditorViewport", nullptr, windowFlags ) )
        {
            Math::Rectangle const viewportRect( ImGui::GetWindowPos(), ImGui::GetWindowSize() );
            viewportSystem.ResizePrimaryViewport( viewportRect );
            m_mouseWithinEditorViewport = ImGui::IsWindowHovered();

            ImGuiX::DrawOrientationGuide( viewportSystem.GetPrimaryViewport() );
        }
        ImGui::End();
    }
}
#endif