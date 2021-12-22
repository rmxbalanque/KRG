#include "EditorWorkspace.h"
#include "Engine/Render/Systems/WorldSystem_WorldRenderer.h"
#include "Engine/Render/Debug/DebugView_Render.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DevUI/OrientationGuide.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Drawing::DrawContext EditorWorkspace::GetDrawingContext()
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

    void EditorWorkspace::DrawDefaultToolbarItems()
    {
        bool const isSavingAllowed = AlwaysAllowSaving() || IsDirty();

        ImGui::BeginDisabled( !isSavingAllowed );
        if ( ImGui::MenuItem( KRG_ICON_FLOPPY_O" Save" ) )
        {
            Save();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled( !CanUndo() );
        if ( ImGui::MenuItem( KRG_ICON_UNDO" Undo" ) )
        {
            Undo();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled( !CanRedo() );
        if ( ImGui::MenuItem( KRG_ICON_REPEAT" Redo" ) )
        {
            Redo();
        }
        ImGui::EndDisabled();
    }

    bool EditorWorkspace::BeginViewportToolbarGroup( char const* pGroupID, ImVec2 const& groupSize )
    {
        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Style::s_backgroundColorSemiLight.Value );
        ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorLight.Value );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 4.0f );
        return ImGui::BeginChild( pGroupID, groupSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar );
    }

    void EditorWorkspace::EndViewportToolbarGroup()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
        ImGui::PopStyleColor( 2 );
    }

    void EditorWorkspace::Initialize( UpdateContext const& context )
    {
        SetDisplayName( m_displayName );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }

    bool EditorWorkspace::DrawViewport( UpdateContext const& context, ViewportInfo const& viewportInfo, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( viewportInfo.m_pViewportRenderTargetTexture != nullptr && viewportInfo.m_retrievePickingID != nullptr );

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
            m_isViewportFocused = ImGui::IsWindowFocused();

            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );

            isViewportFocused = ImGui::IsWindowFocused();
            bool const isWindowHovered = ImGui::IsWindowHovered();
            ImVec2 const windowPos = ImGui::GetWindowPos();

            // Switch focus based on mouse input
            //-------------------------------------------------------------------------

            if ( isWindowHovered )
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

            ImVec2 const viewportImageCursorPos = ImGui::GetCursorPos();
            ImGui::Image( viewportInfo.m_pViewportRenderTargetTexture, viewportSize );

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
                ImGui::SetCursorPos( ImGui::GetWindowContentRegionMin() + ImGui::GetStyle().ItemSpacing );
                ImGui::SetNextItemWidth( 46 );
                if ( ImGui::BeginCombo( "##RenderingOptions", KRG_ICON_EYE, ImGuiComboFlags_HeightLarge ) )
                {
                    auto pRenderWorldSystem = GetWorld()->GetWorldSystem<Render::WorldRendererSystem>();
                    Render::RenderDebugView::DrawRenderVisualizationModesMenu( pRenderWorldSystem );

                    ImGui::EndCombo();
                }

                ImGui::SameLine();

                DrawViewportToolbar( context, pViewport );
            }

            // Handle picking
            //-------------------------------------------------------------------------

            if ( isWindowHovered && !ImGui::IsAnyItemHovered() )
            {
                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
                {
                    ImVec2 const mousePos = ImGui::GetMousePos();
                    if ( mousePos.x != FLT_MAX && mousePos.y != FLT_MAX )
                    {
                        ImVec2 const mousePosWithinViewportImage = ( mousePos - windowPos ) - viewportImageCursorPos;
                        Int2 const pixelCoords = Int2( Math::RoundToInt( mousePosWithinViewportImage.x ), Math::RoundToInt( mousePosWithinViewportImage.y ) );
                        uint64 const pickingID = viewportInfo.m_retrievePickingID( pixelCoords );
                        if ( pickingID != 0 )
                        {
                            OnMousePick( pickingID );
                        }
                    }
                }
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