#include "Workspace_MapEditor.h"
#include "Tools/Entity/Serialization/EntityCollectionDescriptorWriter.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/Helpers/CommonDialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "Engine/Render/Components/LightComponents.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapEditor::EntityMapEditor( EditorContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
        , m_propertyGrid( *context.m_pTypeRegistry, context.m_sourceResourceDirectory )
    {
        m_gizmo.SetTargetTransform( &m_editedTransform );

        m_preEditBindingID = m_propertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PreEdit( eventInfo ); } );
        m_postEditBindingID = m_propertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PostEdit( eventInfo ); } );
    }

    EntityMapEditor::~EntityMapEditor()
    {
        m_propertyGrid.OnPreEdit().Unbind( m_preEditBindingID );
        m_propertyGrid.OnPostEdit().Unbind( m_postEditBindingID );
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
            pfd::message saveChangesMsg( m_editorContext.m_sourceResourceDirectory.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
            if ( saveChangesMsg.result() == pfd::button::yes )
            {
                Save();
            }
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        auto const mapFilename = pfd::save_file( "Create New Map", m_editorContext.m_sourceResourceDirectory.c_str(), { "Map Files", "*.map" } ).result();
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

        ResourceID const mapResourceID = ResourcePath::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, mapFilePath );
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
        auto const selectedMap = pfd::open_file( "Load Map", m_editorContext.m_sourceResourceDirectory.c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
        if ( selectedMap.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( selectedMap[0].c_str() );
        ResourceID const mapToLoad = ResourcePath::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, mapFilePath );
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
                pfd::message saveChangesMsg( m_editorContext.m_sourceResourceDirectory.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
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
            SetDisplayName( m_loadedMap.GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory ).GetFileNameWithoutExtension() );
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

        FileSystem::Path const mapFilePath = SaveDialog( "MAp", m_editorContext.m_sourceResourceDirectory.c_str(), "Map File" );
        if ( !mapFilePath.IsValid() )
        {
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptorWriter writer;
        if ( writer.WriteCollection( *m_editorContext.m_pTypeRegistry, mapFilePath, *pEditedMap ) )
        {
            ResourceID const mapResourcePath = ResourceID::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, mapFilePath );
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

        FileSystem::Path const filePath = m_loadedMap.GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
        EntityCollectionDescriptorWriter writer;
        return writer.WriteCollection( *m_editorContext.m_pTypeRegistry, filePath, *pEditedMap );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::SelectEntity( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr );
        m_pSelectedEntity = pEntity;
        m_pSelectedComponent = nullptr;
        m_propertyGrid.SetTypeToEdit( pEntity );
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
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );
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
    }

    void EntityMapEditor::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        DrawEntityOutliner( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawEntityView( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawPropertyGrid( context );
    }

    void EntityMapEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        bool isInWorldSpace = m_gizmo.IsInWorldSpace();

        InlineString<10> coordinateSpaceSwitcherLabel;
        coordinateSpaceSwitcherLabel.sprintf( "%s###CoordinateSpace", isInWorldSpace ? KRG_ICON_GLOBE : KRG_ICON_DOT_CIRCLE_O );

        if ( ImGui::Selectable( coordinateSpaceSwitcherLabel.c_str(), false, 0, ImVec2( 16, 0 ) ) )
        {
            isInWorldSpace = !isInWorldSpace;
            if ( isInWorldSpace )
            {
                m_gizmo.SwitchToWorldSpace();
            }
            else
            {
                m_gizmo.SwitchToLocalSpace();
            }
        }
        ImGuiX::ItemTooltip( "Current Mode: %s", isInWorldSpace ? "World Space" : "Local Space" );

        //-------------------------------------------------------------------------

        bool t = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Translation;
        bool r = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Rotation;
        bool s = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Scale;

        if ( ImGui::Selectable( KRG_ICON_ARROWS, &t, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
        }
        ImGuiX::ItemTooltip( "Translate" );

        ImGui::SameLine();

        if ( ImGui::Selectable( KRG_ICON_REPEAT, &r, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Rotation );
        }
        ImGuiX::ItemTooltip( "Rotate" );

        ImGui::SameLine();

        if ( ImGui::Selectable( KRG_ICON_EXPAND, &s, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Scale );
        }
        ImGuiX::ItemTooltip( "Scale" );
    }

    void EntityMapEditor::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;

        // Manage Gizmo State
        //-------------------------------------------------------------------------

        auto drawingCtx = GetDrawingContext();

        if ( m_pSelectedEntity != nullptr && m_pSelectedEntity->IsSpatialEntity() )
        {
            drawingCtx.DrawWireBox( m_pSelectedEntity->GetWorldBounds(), Colors::LimeGreen, 2.0f );

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

        auto pOverlayDrawList = ImGui::GetWindowDrawList();

        auto DrawLightButton = [this, pEditedMap, &pViewport, &pOverlayDrawList] ( Render::LightComponent* pLightComponent )
        {
            ImVec2 const iconSize( 48, 48 );
            ImVec2 const buttonOffset( iconSize.x / 2, iconSize.y / 2 );
            ImVec2 const lightPositionScreenSpace = pViewport->WorldSpaceToScreenSpace( pLightComponent->GetPosition() );

            ImGuiX::ScopedFont scopedFont( ImGuiX::Font::Huge );
            ImVec2 const textSize = ImGui::CalcTextSize( KRG_ICON_LIGHTBULB_O );
            ImGui::SetCursorPos( lightPositionScreenSpace - buttonOffset );
            ImGui::PushID( pLightComponent );
            ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0, 0, 0, 0 ) );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0, 0, 0, 0 ) );
            if ( ImGui::Button( KRG_ICON_LIGHTBULB_O, iconSize ) )
            {
                auto pEntity = pEditedMap->FindEntity( pLightComponent->GetEntityID() );
                SelectEntity( pEntity );
                SelectComponent( pLightComponent );
            }
            ImGui::PopStyleColor( 3 );
            ImGui::PopStyleVar( 1 );
            ImGui::PopID();
        };

        auto const& registeredLights = m_pWorld->GetAllRegisteredComponentsOfType<Render::LightComponent>();
        for ( auto pComponent : registeredLights )
        {
            if ( pComponent != m_pSelectedComponent )
            {
                auto pLightComponent = Cast<Render::LightComponent>( pComponent );
                DrawLightButton( const_cast<Render::LightComponent*>( pLightComponent ) );
            }
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
                //drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->GetLightColor(), 2.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + forwardDir, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::SpotLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::SpotLightComponent>( m_pSelectedComponent );
                //drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->GetLightColor(), 2.0f );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->GetLightUmbraAngle(), 1.5f, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::PointLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::PointLightComponent>( m_pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                auto upDir = pLightComponent->GetUpVector();
                auto rightDir = pLightComponent->GetRightVector();

                //drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->GetLightColor(), 2.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( forwardDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( forwardDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( upDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( upDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( rightDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( rightDir * 0.5f ), pLightComponent->GetLightColor(), 3.0f );
            }
        }
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::DrawEntityOutliner( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Entity Outliner" ) )
        {
            auto pEditedMap = ( m_loadedMap.IsValid() ) ? m_pWorld->GetMap( m_loadedMap ) : nullptr;
            if ( pEditedMap != nullptr )
            {
                int32 const numEntities = (int32) pEditedMap->GetEntities().size();
                for ( int32 i = 0; i < numEntities; i++ )
                {
                    Entity* pEntity = pEditedMap->GetEntities()[i];

                    String const buttonLabel = String().sprintf( "%s##%d", pEntity->GetName().c_str(), i );
                    if ( ImGui::Button( buttonLabel.c_str() ) )
                    {
                        SelectEntity( pEntity );
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
        ImGui::End();
    }

    void EntityMapEditor::DrawEntityView( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Entity View" ) )
        {
            if ( m_pSelectedEntity != nullptr )
            {
                ImGui::Text( "Entity Name: %s", m_pSelectedEntity->GetName().c_str() );
                ImGui::Text( "Entity ID: %s", m_pSelectedEntity->GetID().ToString().c_str() );

                ImGui::Separator();

                //-------------------------------------------------------------------------

                if ( m_pSelectedEntity->IsSpatialEntity() )
                {
                    if ( ImGui::CollapsingHeader( "Spatial Info", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        auto const& transform = m_pSelectedEntity->GetWorldTransform();
                        auto const eulerAngles = transform.GetRotation().ToEulerAngles();
                        ImGui::Text( "Rotation: %.2f %.2f %.2f", eulerAngles.m_x, eulerAngles.m_y, eulerAngles.m_z );
                        ImGui::Text( "Translation: %.2f %.2f %.2f", transform.GetTranslation().m_x, transform.GetTranslation().m_y, transform.GetTranslation().m_z );
                        ImGui::Text( "Scale: %.2f %.2f %.2f", transform.GetScale().m_x, transform.GetScale().m_y, transform.GetScale().m_z );
                    }
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
                    if ( auto pSpatialComponent = ComponentCast<SpatialEntityComponent>( pComponent ) )
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

    void EntityMapEditor::DrawPropertyGrid( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Properties" ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::DrawComponentEntry( EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        if ( ImGui::Button( pComponent->GetName().c_str() ) )
        {
            SelectComponent( pComponent );
        }
        ImGui::SameLine();
        ImGui::Text( " - %s - ", pComponent->GetID().ToString().c_str() );
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