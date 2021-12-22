#include "Workspace_MapEditor.h"
#include "Tools/Entity/Serialization/EntityCollectionDescriptorWriter.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/Helpers/CommonDialogs.h"
#include "Game/Core/AI/Components/Component_AISpawn.h"
#include "Engine/Render/Components/Component_Lights.h"
#include "Engine/Core/Components/Component_PlayerSpawn.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapEditor::EntityMapEditor( EditorContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
        , m_propertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        m_gizmo.SetTargetTransform( &m_editedTransform );

        m_preEditBindingID = m_propertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PreEdit( eventInfo ); } );
        m_postEditBindingID = m_propertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PostEdit( eventInfo ); } );

        SetDisplayName( "Map Editor" );
    }

    EntityMapEditor::~EntityMapEditor()
    {
        m_propertyGrid.OnPreEdit().Unbind( m_preEditBindingID );
        m_propertyGrid.OnPostEdit().Unbind( m_postEditBindingID );
    }

    void EntityMapEditor::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );

        m_outlinerWindowName.sprintf( "Map Outliner", GetID() );
        m_entityViewWindowName.sprintf( "Entity Inspector##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::PreEdit( PropertyEditInfo const& eventInfo )
    {
        if ( auto pComponent = TryCast<EntityComponent>( eventInfo.m_pEditedTypeInstance ) )
        {
            m_pWorld->PrepareComponentForEditing( m_loadedMap, pComponent->GetEntityID(), pComponent->GetID() );
        }
    }

    void EntityMapEditor::PostEdit( PropertyEditInfo const& eventInfo )
    {
        if ( auto pComponent = TryCast<EntityComponent>( eventInfo.m_pEditedTypeInstance ) )
        {

        }
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::CreateNewMap()
    {
        // Should we save the current map before unloading?
        if ( IsDirty() )
        {
            pfd::message saveChangesMsg( m_editorContext.GetRawResourceDirectoryPath().c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
            if ( saveChangesMsg.result() == pfd::button::yes )
            {
                Save();
            }
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        auto const mapFilename = pfd::save_file( "Create New Map", m_editorContext.GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" } ).result();
        if ( mapFilename.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( mapFilename.c_str() );
        if ( FileSystem::Exists( mapFilePath ) )
        {
            if ( pfd::message( "Confirm Overwrite", "Map file exists, should we overwrite it?", pfd::choice::yes_no, pfd::icon::error ).result() == pfd::button::no )
            {
                return;
            }
        }

        ResourceID const mapResourceID = ResourcePath::FromFileSystemPath( m_editorContext.GetRawResourceDirectoryPath(), mapFilePath );
        if ( mapResourceID == m_loadedMap )
        {
            pfd::message( "Error", "Cant override currently loaded map!", pfd::choice::ok, pfd::icon::error ).result();
            return;
        }

        if ( mapResourceID.GetResourceTypeID() != EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            pfd::message( "Error", "Invalid map extension provided! Maps need to have the .map extension!", pfd::choice::ok, pfd::icon::error ).result();
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptorWriter writer;
        EntityCollectionDescriptor emptyMap;
        if ( writer.WriteCollection( *m_editorContext.m_pTypeRegistry, mapFilePath, emptyMap ) )
        {
            LoadMap( mapResourceID );
        }
        else
        {
            pfd::message( "Error", "Failed to create new map!", pfd::choice::ok, pfd::icon::error ).result();
        }
    }

    void EntityMapEditor::SelectAndLoadMap()
    {
        auto const selectedMap = pfd::open_file( "Load Map", m_editorContext.GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
        if ( selectedMap.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( selectedMap[0].c_str() );
        ResourceID const mapToLoad = ResourcePath::FromFileSystemPath( m_editorContext.GetRawResourceDirectoryPath(), mapFilePath );
        LoadMap( mapToLoad );
    }

    void EntityMapEditor::LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad )
    {
        if ( mapToLoad.GetResourceID() != m_loadedMap )
        {
            ClearSelection();

            // Should we save the current map before unloading?
            if ( IsDirty() )
            {
                pfd::message saveChangesMsg( m_editorContext.GetRawResourceDirectoryPath().c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
                if ( saveChangesMsg.result() == pfd::button::yes )
                {
                    Save();
                }
            }

            // Unload current map
            if ( m_loadedMap.IsValid() && m_pWorld->IsMapLoaded( m_loadedMap ) )
            {
                m_pWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_loadedMap = mapToLoad.GetResourceID();
            m_pWorld->LoadMap( m_loadedMap );
            SetDisplayName( m_editorContext.ToFileSystemPath( m_loadedMap.GetResourcePath() ).GetFileNameWithoutExtension() );
        }
    }

    void EntityMapEditor::SaveMap()
    {
        Save();
    }

    void EntityMapEditor::SaveMapAs()
    {
        auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;
        if ( pEditedMap == nullptr || !( pEditedMap->IsLoaded() || pEditedMap->IsActivated() ) )
        {
            return;
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        FileSystem::Path const mapFilePath = SaveDialog( "MAp", m_editorContext.GetRawResourceDirectoryPath().c_str(), "Map File" );
        if ( !mapFilePath.IsValid() )
        {
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptorWriter writer;
        if ( writer.WriteCollection( *m_editorContext.m_pTypeRegistry, mapFilePath, *pEditedMap ) )
        {
            ResourceID const mapResourcePath = ResourceID::FromFileSystemPath( m_editorContext.GetRawResourceDirectoryPath(), mapFilePath );
            LoadMap( mapResourcePath );
        }
        else
        {
            pfd::message( "Error", "Failed to save file!", pfd::choice::ok, pfd::icon::error ).result();
        }
    }

    bool EntityMapEditor::Save()
    {
        auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;
        if ( pEditedMap == nullptr || !( pEditedMap->IsLoaded() || pEditedMap->IsActivated() ) )
        {
            return false;
        }

        FileSystem::Path const filePath = m_editorContext.ToFileSystemPath( m_loadedMap.GetResourcePath() );
        EntityCollectionDescriptorWriter writer;
        return writer.WriteCollection( *m_editorContext.m_pTypeRegistry, filePath, *pEditedMap );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::OnGamePreviewStarted()
    {
        m_pWorld->SuspendUpdates();
        m_isGamePreviewRunning = true;
    }

    void EntityMapEditor::OnGamePreviewEnded()
    {
        m_isGamePreviewRunning = false;
        m_pWorld->ResumeUpdates();
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::OnMousePick( uint64 pickingID )
    {
        EntityID const pickedEntityID( pickingID );
        auto pEntity = m_pWorld->FindEntity( pickedEntityID );
        if ( pEntity != nullptr )
        {
            SelectEntity( pEntity );
        }
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::SelectEntity( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr );
        m_pSelectedEntity = pEntity;
        m_pSelectedComponent = nullptr;
        m_propertyGrid.SetTypeToEdit( pEntity );
        m_syncOutlinerToSelectedItem = true;
    }

    void EntityMapEditor::SelectComponent( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pSelectedEntity != nullptr && pComponent != nullptr );
        KRG_ASSERT( m_pSelectedEntity->GetID() == pComponent->GetEntityID() );
        m_pSelectedComponent = pComponent;
        m_propertyGrid.SetTypeToEdit( pComponent );
    }

    void EntityMapEditor::ClearSelection()
    {
        m_pSelectedEntity = nullptr;
        m_pSelectedComponent = nullptr;
        m_propertyGrid.SetTypeToEdit( nullptr );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0, bottomLeftDockID = 0, bottomCenterDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, &bottomCenterDockID, &topDockID );
        ImGui::DockBuilderSplitNode( bottomCenterDockID, ImGuiDir_Right, 0.66f, &bottomCenterDockID, &bottomLeftDockID );
        ImGui::DockBuilderSplitNode( bottomCenterDockID, ImGuiDir_Right, 0.5f, &bottomRightDockID, &bottomCenterDockID );

        // Viewport Flags
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

        // Dock windows
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );
        ImGui::DockBuilderDockWindow( m_outlinerWindowName.c_str(), bottomLeftDockID );
        ImGui::DockBuilderDockWindow( m_entityViewWindowName.c_str(), bottomCenterDockID );
        ImGui::DockBuilderDockWindow( m_propertyGridWindowName.c_str(), bottomRightDockID );
    }

    void EntityMapEditor::DrawWorkspaceToolbar( UpdateContext const& context )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem( "Create New Map" ) )
            {
                CreateNewMap();
            }

            if ( ImGui::MenuItem( "Load Map" ) )
            {
                SelectAndLoadMap();
            }

            if ( ImGui::MenuItem( "Save Map" ) )
            {
                SaveMap();
            }

            if ( ImGui::MenuItem( "Save Map As" ) )
            {
                SaveMapAs();
            }

            ImGui::EndMenu();
        }

        DrawDefaultToolbarItems();
    }

    void EntityMapEditor::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        HandleInput( context );

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        DrawMapOutliner( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawEntityEditor( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawPropertyGrid( context );
    }

    void EntityMapEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( BeginViewportToolbarGroup( "SpatialControls", ImVec2(80, 22 ) ) )
        {
            InlineString<10> const coordinateSpaceSwitcherLabel( InlineString<10>::CtorSprintf(), "%s##CoordinateSpace", m_gizmo.IsInWorldSpace() ? KRG_ICON_GLOBE : KRG_ICON_DOT_CIRCLE_O );
            if ( ImGui::Selectable( coordinateSpaceSwitcherLabel.c_str(), false, 0, ImVec2( 14, 0 ) ) )
            {
                m_gizmo.SetCoordinateSystemSpace( m_gizmo.IsInWorldSpace() ? CoordinateSpace::Local : CoordinateSpace::World );
            }
            ImGuiX::ItemTooltip( "Current Mode: %s", m_gizmo.IsInWorldSpace() ? "World Space" : "Local Space" );

            ImGuiX::VerticalSeparator( ImVec2( 6, -1 ) );

            //-------------------------------------------------------------------------

            bool t = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Translation;
            bool r = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Rotation;
            bool s = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Scale;

            if ( ImGui::Selectable( KRG_ICON_ARROWS, t, 0, ImVec2( 14, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
            }
            ImGuiX::ItemTooltip( "Translate" );

            ImGui::SameLine();

            if ( ImGui::Selectable( KRG_ICON_REPEAT, r, 0, ImVec2( 14, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Rotation );
            }
            ImGuiX::ItemTooltip( "Rotate" );

            ImGui::SameLine();

            if ( ImGui::Selectable( KRG_ICON_EXPAND, s, 0, ImVec2( 14, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Scale );
            }
            ImGuiX::ItemTooltip( "Scale" );
        }
        EndViewportToolbarGroup();
    }

    void EntityMapEditor::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;

        // Game Preview
        //-------------------------------------------------------------------------

        if ( m_isGamePreviewRunning )
        {
            constexpr static char const* const gamePreviewMsg = "Game Preview Running";

            ImGuiX::ScopedFont const sf( ImGuiX::Font::Huge, Colors::Red );
            ImVec2 const textSize = ImGui::CalcTextSize( gamePreviewMsg );
            ImVec2 const msgPos = ( ImGui::GetWindowSize() - textSize ) / 2;

            ImGui::SetCursorPos( msgPos );
            ImGui::Text( gamePreviewMsg );
            return;
        }

        // Selection and Manipulation
        //-------------------------------------------------------------------------

        auto drawingCtx = GetDrawingContext();

        if ( m_pSelectedEntity != nullptr && m_pSelectedEntity->IsSpatialEntity() )
        {
            drawingCtx.DrawWireBox( m_pSelectedEntity->GetWorldBounds(), Colors::Yellow, 3.0f, Drawing::EnableDepthTest );
        }

        if ( m_pSelectedEntity != nullptr && m_pSelectedEntity->IsSpatialEntity() )
        {
            // If we have a component selected, manipulate it
            if ( m_pSelectedComponent != nullptr && m_pSelectedComponent->IsInitialized() )
            {
                auto pSpatialComponent = TryCast<SpatialEntityComponent>( m_pSelectedComponent );
                if ( pSpatialComponent != nullptr && pSpatialComponent->IsInitialized() )
                {
                    Transform originalTransform = m_editedTransform;
                    m_editedTransform = pSpatialComponent->GetWorldTransform();
                    if ( m_gizmo.Draw( *m_pWorld->GetViewport() ) )
                    {
                        pSpatialComponent->SetWorldTransform( m_editedTransform );
                    }
                }
            }
            else // Directly manipulated entity
            {
                m_editedTransform = m_pSelectedEntity->GetWorldTransform();
                if ( m_gizmo.Draw( *m_pWorld->GetViewport() ) )
                {
                    m_pSelectedEntity->SetWorldTransform( m_editedTransform );
                }
            }

            // Ensure that we always show a gizmo, when selecting a spatial entity
            if ( m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::None )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
            }
        }

        // Overlay Widgets
        //-------------------------------------------------------------------------

        auto DrawComponentWorldIcon = [this, pViewport] ( SpatialEntityComponent* pComponent, char icon[4] )
        {
            if ( pViewport->IsWorldSpacePointVisible( pComponent->GetPosition() ) )
            {
                ImVec2 const positionScreenSpace = pViewport->WorldSpaceToScreenSpace( pComponent->GetPosition() );
                if ( ImGuiX::DrawOverlayIcon( positionScreenSpace, icon, pComponent ) )
                {
                    return true;
                }
            }

            return false;
        };

        auto const& registeredLights = m_pWorld->GetAllRegisteredComponentsOfType<Render::LightComponent>();
        for ( auto pComponent : registeredLights )
        {
            if ( pComponent != m_pSelectedComponent )
            {
                auto pLightComponent = const_cast<Render::LightComponent*>( pComponent );
                if ( DrawComponentWorldIcon( pLightComponent, KRG_ICON_LIGHTBULB_O ) )
                {
                    auto pEntity = pEditedMap->FindEntity( pLightComponent->GetEntityID() );
                    SelectEntity( pEntity );
                    SelectComponent( pLightComponent );
                }
            }
        }

        auto const& registeredPlayerSpawns = m_pWorld->GetAllRegisteredComponentsOfType<Player::PlayerSpawnComponent>();
        for ( auto pComponent : registeredPlayerSpawns )
        {
            if ( pComponent != m_pSelectedComponent )
            {
                auto pSpawnComponent = const_cast<Player::PlayerSpawnComponent*>( pComponent );
                if ( DrawComponentWorldIcon( pSpawnComponent, KRG_ICON_GAMEPAD ) )
                {
                    auto pEntity = pEditedMap->FindEntity( pSpawnComponent->GetEntityID() );
                    SelectEntity( pEntity );
                    SelectComponent( pSpawnComponent );
                }
            }

            auto const& spawnTransform = pComponent->GetWorldTransform();
            drawingCtx.DrawArrow( spawnTransform.GetTranslation(), spawnTransform.GetForwardVector(), 0.5f, Colors::Yellow, 3.0f );
        }

        auto const& registeredAISpawns = m_pWorld->GetAllRegisteredComponentsOfType<AI::AISpawnComponent>();
        for ( auto pComponent : registeredAISpawns )
        {
            if ( pComponent != m_pSelectedComponent )
            {
                auto pSpawnComponent = const_cast<AI::AISpawnComponent*>( pComponent );
                if ( DrawComponentWorldIcon( pSpawnComponent, KRG_ICON_USER_SECRET ) )
                {
                    auto pEntity = pEditedMap->FindEntity( pSpawnComponent->GetEntityID() );
                    SelectEntity( pEntity );
                    SelectComponent( pSpawnComponent );
                }
            }

            auto const& spawnTransform = pComponent->GetWorldTransform();
            drawingCtx.DrawArrow( spawnTransform.GetTranslation(), spawnTransform.GetForwardVector(), 0.5f, Colors::Yellow, 3.0f );
        }

        // Draw light debug widgets
        //-------------------------------------------------------------------------
        // TODO: generalized component visualizers

        if ( m_pSelectedComponent != nullptr )
        {
            if ( IsOfType<Render::DirectionalLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::DirectionalLightComponent>( m_pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + forwardDir, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::SpotLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::SpotLightComponent>( m_pSelectedComponent );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->GetLightInnerUmbraAngle(), 1.5f, pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->GetLightOuterUmbraAngle(), 1.5f, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::PointLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::PointLightComponent>( m_pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                auto upDir = pLightComponent->GetUpVector();
                auto rightDir = pLightComponent->GetRightVector();

                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( forwardDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( forwardDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( upDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( upDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( rightDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( rightDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
            }
        }
    }

    void EntityMapEditor::DrawPropertyGrid( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_propertyGridWindowName.c_str() ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::HandleInput( UpdateContext const& context )
    {
        if ( m_isViewportFocused )
        {
            if ( ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Space ) ) )
            {
                m_gizmo.SwitchToNextMode();
            }
        }
    }
}

//-------------------------------------------------------------------------
// Map Outliner
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityMapEditor::DrawMapOutliner( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_outlinerWindowName.c_str() ) )
        {
            // Create New Entity
            //-------------------------------------------------------------------------

            {
                ImGuiX::ScopedFont const sf( ImGuiX::Font::SmallBold );
                if ( ImGuiX::ColoredButton( Colors::Green, Colors::White, "CREATE ENTITY", ImVec2( -1, 0 ) ) )
                {
                    Entity* pEntity = KRG::New<Entity>( StringID( "New Entity" ) );
                    m_pWorld->GetMap( m_loadedMap )->AddEntity( pEntity );
                    SelectEntity( pEntity );
                }
            }

            //-------------------------------------------------------------------------

            if ( ImGui::BeginChild( "EntityList", ImVec2( -1, 0 ) ) )
            {
                auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;
                if ( pEditedMap != nullptr )
                {
                    int32 const numEntities = (int32) pEditedMap->GetEntities().size();
                    for ( int32 i = 0; i < numEntities; i++ )
                    {
                        Entity* pEntity = pEditedMap->GetEntities()[i];

                        bool const isSelected = pEntity == m_pSelectedEntity;
                        if ( isSelected )
                        {
                            ImGuiX::PushFontAndColor( ImGuiX::Font::SmallBold, Colors::Yellow );
                        }

                        String const buttonLabel( String::CtorSprintf(), "%s##%u", pEntity->GetName().c_str(), pEntity->GetID().ToUint64() );
                        if ( ImGui::Selectable( buttonLabel.c_str(), isSelected, 0 ) )
                        {
                            SelectEntity( pEntity );
                        }

                        if ( isSelected )
                        {
                            ImGui::PopFont();
                            ImGui::PopStyleColor();
                        }

                        if ( isSelected && m_syncOutlinerToSelectedItem )
                        {
                            ImGui::ScrollToItem();
                            m_syncOutlinerToSelectedItem = false;
                        }

                        if ( ImGui::IsItemHovered() )
                        {
                            if ( pEntity->IsSpatialEntity() )
                            {
                                auto drawingCtx = GetDrawingContext();
                                drawingCtx.DrawWireBox( pEntity->GetWorldBounds(), Colors::Yellow, 2.0f );
                            }
                        }
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

//-------------------------------------------------------------------------
// Entity Inspector
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityMapEditor::DrawEntityEditor( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_entityViewWindowName.c_str() ) )
        {
            if ( m_pSelectedEntity != nullptr )
            {
                {
                    ImGuiX::ScopedFont sf( ImGuiX::Font::Large );
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "%s", m_pSelectedEntity->GetName().c_str() );
                }

                ImGui::SameLine( ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x - 22, 0);
                if ( ImGui::Button( KRG_ICON_PENCIL, ImVec2( 22, 0 ) ) )
                {
                    SelectEntity( m_pSelectedEntity );
                }
                ImGuiX::ItemTooltip( "Edit Entity Details" );

                if ( m_pSelectedEntity->IsSpatialEntity() )
                {
                    ImGuiX::DisplayTransform( m_pSelectedEntity->GetWorldTransform(), ImGui::GetContentRegionAvail().x );
                }

                //-------------------------------------------------------------------------

                if ( !m_pSelectedEntity->GetSystems().empty() )
                {
                    if ( ImGui::CollapsingHeader( "Systems", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        for ( auto pSystem : m_pSelectedEntity->GetSystems() )
                        {
                            ImGui::Text( pSystem->GetName() );
                        }
                    }
                }

                //-------------------------------------------------------------------------

                TInlineVector<EntityComponent*, 10> components;
                for ( auto pComponent : m_pSelectedEntity->GetComponents() )
                {
                    if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
                    {
                        continue;
                    }

                    components.emplace_back( pComponent );
                }

                if ( !components.empty() )
                {
                    if ( ImGui::CollapsingHeader( "Components", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        for ( auto pComponent : components )
                        {
                            DrawComponentEntry( pComponent );
                        }
                    }
                }

                //-------------------------------------------------------------------------

                if ( m_pSelectedEntity->m_pRootSpatialComponent != nullptr )
                {
                    if ( ImGui::CollapsingHeader( "Spatial Components", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        DrawSpatialComponentTree( m_pSelectedEntity->m_pRootSpatialComponent );
                    }
                }
            }
        }
        ImGui::End();
    }

    void EntityMapEditor::DrawComponentEntry( EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        if ( ImGui::Button( pComponent->GetName().c_str() ) )
        {
            SelectComponent( pComponent );
        }
        ImGui::SameLine();

        EntityComponent::Status const componentStatus = pComponent->GetStatus();

        switch ( componentStatus )
        {
            case EntityComponent::Status::LoadingFailed:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF0000FF );
                ImGui::Text( "Load Failed" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Unloaded:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF666666 );
                ImGui::Text( "Unloaded" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Loading:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFFAAAAAA );
                ImGui::Text( "Loading" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Loaded:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF00FFFF );
                ImGui::Text( "Loaded" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Initialized:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF00FFFF );
                ImGui::Text( "Initialized" );
                ImGui::PopStyleColor( 1 );
            }
            break;
        }
    }

    void EntityMapEditor::DrawSpatialComponentTree( SpatialEntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        bool const IsNodeExpanded = ImGui::TreeNodeEx( pComponent, ImGuiTreeNodeFlags_DefaultOpen, "" );
        ImGui::SameLine();
        DrawComponentEntry( pComponent );

        if ( IsNodeExpanded )
        {
            for ( auto pChildComponent : pComponent->m_spatialChildren )
            {
                DrawSpatialComponentTree( pChildComponent );
            }

            ImGui::TreePop();
        }
    }
}