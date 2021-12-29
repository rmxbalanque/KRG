#include "EditorWorkspace.h"
#include "Engine/Render/Debug/DebugView_Render.h"
#include "Engine/Core/DebugViews/DebugView_Camera.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/ToolsUI/OrientationGuide.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Drawing::DrawContext EditorWorkspace::GetDrawingContext()
    {
        return m_pWorld->GetDebugDrawingSystem()->GetDrawingContext();
    }

    //-------------------------------------------------------------------------

    EditorWorkspace::EditorWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, String const& displayName )
        : m_pTypeRegistry( context.m_pTypeRegistry )
        , m_pResourceDatabase( context.m_pResourceDatabase )
        , m_pResourceSystem( context.m_pResourceSystem )
        , m_pWorld( pWorld )
        , m_displayName( displayName )
    {
        KRG_ASSERT( m_pWorld != nullptr );
        KRG_ASSERT( m_pTypeRegistry != nullptr && m_pResourceDatabase != nullptr && m_pResourceSystem != nullptr );
    }

    EditorWorkspace::~EditorWorkspace()
    {
        KRG_ASSERT( m_requestedResources.empty() );
        KRG_ASSERT( m_reloadingResources.empty() );
    }

    void EditorWorkspace::Initialize( UpdateContext const& context )
    {
        SetDisplayName( m_displayName );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }

    //-------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------

    void EditorWorkspace::DrawDefaultViewportToolbarItems()
    {
        ImGui::SetNextItemWidth( 44 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 5, 5 ) );
        if ( ImGui::BeginCombo( "##RenderingOptions", KRG_ICON_EYE, ImGuiComboFlags_HeightLarge ) )
        {
            Render::RenderDebugView::DrawRenderVisualizationModesMenu( GetWorld() );
            ImGui::EndCombo();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( 44 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 5, 5 ) );
        if ( ImGui::BeginCombo( "##CameraOptions", KRG_ICON_CAMERA, ImGuiComboFlags_HeightLarge ) )
        {
            CameraDebugView::DrawDebugCameraOptions( GetWorld() );

            ImGui::EndCombo();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();
    }

    bool EditorWorkspace::BeginViewportToolbarGroup( char const* pGroupID, ImVec2 groupSize )
    {
        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Style::s_backgroundColorSemiLight.Value );
        ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorLight.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, ImGuiX::Style::s_itemColorDark.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBgActive, ImGuiX::Style::s_backgroundColorMedium.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, ImGuiX::Style::s_backgroundColorMedium.Value );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 4.0f );

        // Adjust "use available" height to default toolbar height
        if ( groupSize.y <= 0 )
        {
            groupSize.y = ImGui::GetFrameHeight();
        }

        return ImGui::BeginChild( pGroupID, groupSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar );
    }

    void EditorWorkspace::EndViewportToolbarGroup()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
        ImGui::PopStyleColor( 5 );
    }

    //-------------------------------------------------------------------------

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
                DrawDefaultViewportToolbarItems();
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

    //-------------------------------------------------------------------------

    void EditorWorkspace::LoadResource( Resource::ResourcePtr* pResourcePtr )
    {
        KRG_ASSERT( pResourcePtr != nullptr && pResourcePtr->IsUnloaded() );
        KRG_ASSERT( !VectorContains( m_requestedResources, pResourcePtr ) );
        m_requestedResources.emplace_back( pResourcePtr );
        m_pResourceSystem->LoadResource( *pResourcePtr );
    }

    void EditorWorkspace::UnloadResource( Resource::ResourcePtr* pResourcePtr )
    {
        KRG_ASSERT( !pResourcePtr->IsUnloaded() );
        KRG_ASSERT( VectorContains( m_requestedResources, pResourcePtr ) );
        m_pResourceSystem->UnloadResource( *pResourcePtr );
        m_requestedResources.erase_first_unsorted( pResourcePtr );
    }

    void EditorWorkspace::AddEntityToWorld( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && !pEntity->GetCollectionID().IsValid() );
        KRG_ASSERT( !VectorContains( m_addedEntities, pEntity ) );
        m_addedEntities.emplace_back( pEntity );
        m_pWorld->GetPersistentMap()->AddEntity( pEntity );
    }

    void EditorWorkspace::RemoveEntityFromWorld( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->GetCollectionID().IsValid() );
        KRG_ASSERT( VectorContains( m_addedEntities, pEntity ) );
        m_pWorld->GetPersistentMap()->RemoveEntity( pEntity );
        m_addedEntities.erase_first_unsorted( pEntity );
    }

    void EditorWorkspace::DestroyEntityInWorld( Entity*& pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->GetCollectionID().IsValid() );
        KRG_ASSERT( VectorContains( m_addedEntities, pEntity ) );
        m_pWorld->GetPersistentMap()->DestroyEntity( pEntity );
        m_addedEntities.erase_first_unsorted( pEntity );
        pEntity = nullptr;
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded )
    {
        for ( auto& pLoadedResource : m_requestedResources )
        {
            if ( pLoadedResource->IsUnloaded() || pLoadedResource->HasLoadingFailed() )
            {
                continue;
            }

            // Check resource and install dependencies to see if we need to unload it
            bool shouldUnload = VectorContains( resourcesToBeReloaded, pLoadedResource->GetResourceID() );
            if ( !shouldUnload )
            {
                for ( ResourceID const& installDependency : pLoadedResource->GetInstallDependencies() )
                {
                    if ( VectorContains( resourcesToBeReloaded, installDependency ) )
                    {
                        shouldUnload = true;
                        break;
                    }
                }
            }

            // Request unload and track the resource we need to reload
            if ( shouldUnload )
            {
                m_pResourceSystem->UnloadResource( *pLoadedResource );
                m_reloadingResources.emplace_back( pLoadedResource );
            }
        }
    }

    void EditorWorkspace::EndHotReload()
    {
        for( auto& pReloadedResource : m_reloadingResources )
        {
            m_pResourceSystem->LoadResource( *pReloadedResource );
        }
        m_reloadingResources.clear();
    }
}