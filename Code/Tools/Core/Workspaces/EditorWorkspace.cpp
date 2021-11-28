#include "EditorWorkspace.h"
#include "Engine/Render/WorldSystems/WorldRendererSystem.h"
#include "Engine/Render/Debug/DebugView_Render.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DevUI/OrientationGuide.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Debug::DrawingContext EditorWorkspace::GetDrawingContext()
    {
        return m_pWorld->GetDebugDrawingSystem()->GetDrawingContext();
    }

    //-------------------------------------------------------------------------

    EditorWorkspace::EditorWorkspace( EditorContext const& context, EntityWorld* pWorld, String const& displayName )
        : m_editorContext( context )
        , m_pWorld( pWorld )
        , m_displayName( displayName )
    {
        KRG_ASSERT( m_pWorld != nullptr );
    }

    void EditorWorkspace::SetDisplayName( String const& name )
    {
        m_displayName = name;
        m_workspaceWindowID.sprintf( "%s###window%u", m_displayName.c_str(), GetID() );
    }

    void EditorWorkspace::Initialize( UpdateContext const& context )
    {
        SetDisplayName( m_displayName );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }

    bool EditorWorkspace::DrawViewport( UpdateContext const& context, ImTextureID pViewportRenderTargetTexture, ImGuiWindowClass* pWindowClass )
    {
        auto pWorld = GetWorld();

        Render::Viewport* pViewport = pWorld->GetViewport();
        bool isViewportFocused = false;
        ImGuiWindowFlags const viewportWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        // Create viewport window
        ImGui::SetNextWindowClass( pWindowClass );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        if ( ImGui::Begin( GetViewportWindowID(), nullptr, viewportWindowFlags ) )
        {
            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );
            isViewportFocused = ImGui::IsWindowFocused();

            // Switch focus based on mouse input
            //-------------------------------------------------------------------------

            if ( ImGui::IsWindowHovered() )
            {
                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) || ImGui::IsMouseClicked( ImGuiMouseButton_Middle ) )
                {
                    ImGui::SetWindowFocus();
                }
            }

            // Update engine viewport dimensions
            //-------------------------------------------------------------------------

            Math::Rectangle const viewportRect( Float2::Zero, viewportSize );
            pViewport->Resize( viewportRect );

            // Draw 3D scene
            //-------------------------------------------------------------------------

            ImGui::Image( pViewportRenderTargetTexture, viewportSize );

            // Draw overlay elements
            //-------------------------------------------------------------------------

            ImGui::SetCursorPos( style.WindowPadding );
            DrawViewportOverlayElements( context, pViewport );

            if ( HasViewportOrientationGuide() )
            {
                ImGuiX::OrientationGuide::Draw( ImGui::GetWindowPos() + viewportSize - ImVec2( ImGuiX::OrientationGuide::GetWidth() + 4, ImGuiX::OrientationGuide::GetWidth() + 4 ), *pViewport );
            }

            // Draw viewport toolbar
            //-------------------------------------------------------------------------

            if ( HasViewportToolbar() )
            {
                ImGui::SetCursorPos( ImGui::GetStyle().ItemSpacing );
                ImGui::SetNextItemWidth( 46 );
                if ( ImGui::BeginCombo( "##Test", KRG_ICON_EYE, ImGuiComboFlags_HeightLarge ) )
                {
                    auto pRenderWorldSystem = GetWorld()->GetWorldSystem<Render::WorldRendererSystem>();
                    Render::RenderDebugView::DrawRenderVisualizationModesMenu( pRenderWorldSystem );

                    ImGui::EndCombo();
                }

                ImGui::SameLine();

                DrawViewportToolbar( context, pViewport );
                ImGui::EndMenu();
            }

            // Handle being docked
            //-------------------------------------------------------------------------

            if ( auto pDockNode = ImGui::GetWindowDockNode() )
            {
                pDockNode->LocalFlags = 0;
                pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
                pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        return isViewportFocused;
    }
}