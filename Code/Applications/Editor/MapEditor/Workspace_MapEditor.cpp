#include "Workspace_MapEditor.h"
#include "Tools/Entity/Serialization/EntityCollectionDescriptorWriter.h"
#include "Tools/Core/Editor/EditorWorkspace.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapEditor::EntityMapEditor( EditorContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
        , m_propertyGrid( *context.m_pTypeRegistry, context.m_sourceResourceDirectory )
    {
        m_gizmo.SetTargetTransform( &m_editedTransform );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::CreateNewMap()
    {

    }

    void EntityMapEditor::SelectAndLoadMap()
    {
        auto const selectedMap = pfd::open_file( "Choose Map", m_editorContext.m_sourceResourceDirectory.c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
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
                // Save?
            }

            // Unload current map
            if ( m_loadedMap.IsValid() && m_pWorld->IsMapLoaded( m_loadedMap ) )
            {
                m_pWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_loadedMap = mapToLoad.GetResourceID();
            m_pWorld->LoadMap( m_loadedMap );
        }
    }

    void EntityMapEditor::SaveMap()
    {
        Save();
    }

    void EntityMapEditor::SaveMapAs()
    {

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

    void EntityMapEditor::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        DrawEntityOutliner( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawComponentsView( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawPropertyGrid( context );
    }

    void EntityMapEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        bool a = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Translation;
        bool b = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Rotation;
        bool c = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Scale;
        
        if ( ImGui::Selectable( KRG_ICON_ARROWS, &a, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
        }

        ImGui::SameLine();

        if ( ImGui::Selectable( KRG_ICON_REPEAT, &b, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Rotation );
        }

        ImGui::SameLine();

        if ( ImGui::Selectable( KRG_ICON_EXPAND, &c, 0, ImVec2( 16, 0 ) ) )
        {
            m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Scale );
        }
    }

    void EntityMapEditor::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        // Manage Gizmo State
        //-------------------------------------------------------------------------

        auto drawingCtx = GetDrawingContext();

        if ( m_pSelectedEntity != nullptr && m_pSelectedEntity->IsSpatialEntity() )
        {
            drawingCtx.DrawWireBox( m_pSelectedEntity->GetWorldBounds(), Colors::LimeGreen, 2.0f );

            // If we have a component selected, manipulate it
            if ( m_pSelectedComponent != nullptr )
            {
                auto pSpatialComponent = TryCast<SpatialEntityComponent>( m_pSelectedComponent );
                if ( pSpatialComponent != nullptr )
                {
                    m_editedTransform = pSpatialComponent->GetWorldTransform();
                    m_gizmo.Draw( *m_pWorld->GetViewport() );
                    pSpatialComponent->SetWorldTransform( m_editedTransform );
                }
            }
            else // Directly manipulated entity
            {
                m_editedTransform = m_pSelectedEntity->GetWorldTransform();
                m_gizmo.Draw( *m_pWorld->GetViewport() );
                m_pSelectedEntity->SetWorldTransform( m_editedTransform );
            }
        }
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::DrawEntityOutliner( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Entities" ) )
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
                        m_pSelectedEntity = pEntity;
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

    void EntityMapEditor::DrawComponentsView( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Components" ) )
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
            m_pSelectedComponent = pComponent;
            m_propertyGrid.SetTypeToEdit( m_pSelectedComponent );
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