#include "EntityEditor_BaseWorkspace.h"

#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Navmesh/DebugViews/DebugView_Navmesh.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/PhysicsScene.h"
#include "Engine/Physics/PhysicsLayers.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "Engine/Core/Entity/EntitySerialization.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Math/Random.h"
#include "System/Core/Logging/Log.h"

#include "Game/Core/AI/Components/Component_AISpawn.h"
#include "Engine/Physics/Components/Component_PhysicsMesh.h"
#include "Engine/Render/Components/Component_StaticMesh.h"
#include "Engine/Render/Components/Component_Lights.h"
#include "Engine/Core/Components/Component_Volumes.h"
#include "Engine/Core/Components/Component_PlayerSpawn.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityEditorBaseWorkspace::EntityEditorBaseWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
        , m_context( *context.m_pTypeRegistry, *context.m_pResourceDatabase, pWorld, m_undoStack )
        , m_entityOutliner( m_context )
        , m_entityInspector( m_context )
        , m_propertyGrid( m_context )
    {
        m_gizmo.SetTargetTransform( &m_gizmoTransform );
    }

    //-------------------------------------------------------------------------

    void EntityEditorBaseWorkspace::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );

        m_volumeTypes = context.GetSystem<TypeSystem::TypeRegistry>()->GetAllDerivedTypes( VolumeComponent::GetStaticTypeID(), false, false, true );

        m_outlinerWindowName.sprintf( "Map Outliner##%u", GetID() );
        m_entityViewWindowName.sprintf( "Entity Inspector##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
    }

    void EntityEditorBaseWorkspace::Shutdown( UpdateContext const& context )
    {
        EditorWorkspace::Shutdown( context );
    }

    void EntityEditorBaseWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
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

    //-------------------------------------------------------------------------

    void EntityEditorBaseWorkspace::OnUndoRedo( UndoStack::Operation operation, IUndoableAction const* pAction )
    {
        m_context.OnUndoRedo( operation, pAction );
    }

    void EntityEditorBaseWorkspace::OnDragAndDrop( Render::Viewport* pViewport )
    {
        if ( ImGuiPayload const* payload = ImGui::AcceptDragDropPayload( "ResourceFile", ImGuiDragDropFlags_AcceptBeforeDelivery ) )
        {
            InlineString payloadStr = (char*) payload->Data;

            ResourceID const resourceID( payloadStr.c_str() );
            if ( !resourceID.IsValid() || !m_pResourceDatabase->DoesResourceExist( resourceID ) )
            {
                return;
            }

            if ( !payload->IsDelivery() )
            {
                return;
            }

            // Calculate world position for new entity
            //-------------------------------------------------------------------------

            Physics::Scene* pPhysicsScene = m_pWorld->GetWorldSystem<Physics::PhysicsWorldSystem>()->GetScene();

            Vector const WSNear = pViewport->ScreenSpaceToWorldSpaceNearPlane( ImGui::GetMousePos() - ImGui::GetWindowPos() );
            Vector const WSFar = pViewport->ScreenSpaceToWorldSpaceFarPlane( ImGui::GetMousePos() - ImGui::GetWindowPos() );
            Vector worldPosition = Vector::Zero;

            Physics::RayCastResults results;
            Physics::QueryFilter filter( Physics::CreateLayerMask( Physics::Layers::Environment ) );
            pPhysicsScene->AcquireReadLock();
            if ( pPhysicsScene->RayCast( WSNear, WSFar, filter, results ) )
            {
                worldPosition = results.GetHitPosition();
            }
            else // Arbitrary position
            {
                worldPosition = WSNear + ( ( WSFar - WSNear ).GetNormalized3() * 10.0f );
            }
            pPhysicsScene->ReleaseReadLock();

            // Try add resource to map
            //-------------------------------------------------------------------------

            DropResourceInMap( resourceID, worldPosition );
        }
    }

    void EntityEditorBaseWorkspace::DropResourceInMap( ResourceID const& resourceID, Vector const& worldPosition )
    {
        // Static Mesh Resource
        //-------------------------------------------------------------------------

        if ( resourceID.GetResourceTypeID() == Render::StaticMesh::GetStaticResourceTypeID() )
        {
            // Create entity
            StringID const entityName( resourceID.GetFileNameWithoutExtension().c_str() );
            auto pMeshEntity = KRG::New<Entity>( entityName );

            // Create mesh component
            auto pMeshComponent = KRG::New<Render::StaticMeshComponent>( StringID( "Mesh Component" ) );
            pMeshComponent->SetMesh( resourceID );
            pMeshComponent->SetWorldTransform( Transform( Quaternion::Identity, worldPosition ) );
            pMeshEntity->AddComponent( pMeshComponent );

            // Try create optional physics collision component
            ResourcePath physicsResourcePath = resourceID.GetResourcePath();
            physicsResourcePath.ReplaceExtension( Physics::PhysicsMesh::GetStaticResourceTypeID().ToString() );
            ResourceID const physicsResourceID( physicsResourcePath );
            if ( m_pResourceDatabase->DoesResourceExist( physicsResourceID ) )
            {
                auto pPhysicsMeshComponent = KRG::New<Physics::PhysicsMeshComponent>( StringID( "Physics Component" ) );
                pPhysicsMeshComponent->SetMesh( physicsResourceID );
                pMeshEntity->AddComponent( pPhysicsMeshComponent );
            }

            // Add entity to map
            m_context.AddEntity( pMeshEntity );
        }
    }

    void EntityEditorBaseWorkspace::OnMousePick( Render::PickingID pickingID )
    {
        EntityID const pickedEntityID( pickingID.m_0 );
        auto pEntity = m_pWorld->FindEntity( pickedEntityID );
        if ( pEntity == nullptr )
        {
            return;
        }

        // If we have ctrl-held, select the individual component
        if ( ImGui::GetIO().KeyAlt )
        {
            ComponentID const pickedComponentID( pickingID.m_1 );
            auto pComponent = pEntity->FindComponent( pickedComponentID );
            KRG_ASSERT( pComponent != nullptr );
            m_context.SelectEntity( pEntity );
            m_context.SelectComponent( pComponent );
        }
        else if ( ImGui::GetIO().KeyShift || ImGui::GetIO().KeyCtrl )
        {
            if ( m_context.IsSelected( pEntity ) )
            {
                m_context.RemoveFromSelectedEntities( pEntity );
            }
            else
            {
                m_context.AddToSelectedEntities( pEntity );
            }
        }
        else
        {
            m_context.SelectEntity( pEntity );
        }
    }

    //-------------------------------------------------------------------------

    void EntityEditorBaseWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        constexpr float const buttonWidth = 16;

        if ( BeginViewportToolbarGroup( "SpatialControls", ImVec2( 90, 23 ) ) )
        {
            TInlineString<10> const coordinateSpaceSwitcherLabel( TInlineString<10>::CtorSprintf(), "%s##CoordinateSpace", m_gizmo.IsInWorldSpace() ? KRG_ICON_GLOBE_AFRICA : KRG_ICON_MAP_MARKER_ALT );
            if ( ImGui::Selectable( coordinateSpaceSwitcherLabel.c_str(), false, 0, ImVec2( buttonWidth, 0 ) ) )
            {
                m_gizmo.SetCoordinateSystemSpace( m_gizmo.IsInWorldSpace() ? CoordinateSpace::Local : CoordinateSpace::World );
            }
            ImGuiX::ItemTooltip( "Current Mode: %s", m_gizmo.IsInWorldSpace() ? "World Space" : "Local Space" );

            ImGuiX::VerticalSeparator( ImVec2( 9, -1 ), ImGuiX::Style::s_itemColorLight );

            //-------------------------------------------------------------------------

            bool t = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Translation;
            bool r = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Rotation;
            bool s = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Scale;

            if ( ImGui::Selectable( KRG_ICON_ARROWS_ALT, t, 0, ImVec2( buttonWidth, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
            }
            ImGuiX::ItemTooltip( "Translate" );

            ImGui::SameLine();

            if ( ImGui::Selectable( KRG_ICON_SYNC_ALT, r, 0, ImVec2( buttonWidth, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Rotation );
            }
            ImGuiX::ItemTooltip( "Rotate" );

            ImGui::SameLine();

            if ( ImGui::Selectable( KRG_ICON_EXPAND, s, 0, ImVec2( buttonWidth, 0 ) ) )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Scale );
            }
            ImGuiX::ItemTooltip( "Scale" );
        }
        EndViewportToolbarGroup();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( 46 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
        if ( ImGui::BeginCombo( "##MapEditorOptions", KRG_ICON_COG, ImGuiComboFlags_HeightLarge ) )
        {
            ImGuiX::TextSeparator( "Physics" );

            auto pPhysicsWorldSystem = m_pWorld->GetWorldSystem<Physics::PhysicsWorldSystem>();
            bool isDebugEnabled = pPhysicsWorldSystem->IsDebugDrawingEnabled();
            if ( ImGui::MenuItem( "Show Physics Collision", nullptr, &isDebugEnabled ) )
            {
                pPhysicsWorldSystem->SetDebugDrawingEnabled( isDebugEnabled );
            }

            //-------------------------------------------------------------------------

            ImGuiX::TextSeparator( "Navmesh" );

            auto pNavmeshWorldSystem = m_pWorld->GetWorldSystem<Navmesh::NavmeshWorldSystem>();
            Navmesh::NavmeshDebugView::DrawNavmeshDebugSettings( pNavmeshWorldSystem );

            //-------------------------------------------------------------------------

            ImGui::EndCombo();
        }
        ImGuiX::ItemTooltip( "Map Visualizations" );
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        ImGui::SameLine();
        ImGui::SetNextItemWidth( 46 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
        if ( ImGui::BeginCombo( "##Volumes", KRG_ICON_CUBE, ImGuiComboFlags_HeightLarge ) )
        {
            for ( auto pVolumeTypeInfo : m_volumeTypes )
            {
                bool isVisualized = VectorContains( m_visualizedVolumeTypes, pVolumeTypeInfo );
                if ( ImGui::MenuItem( pVolumeTypeInfo->GetFriendlyTypeName(), nullptr, isVisualized ) )
                {
                    if ( isVisualized )
                    {
                        m_visualizedVolumeTypes.erase_first( pVolumeTypeInfo );
                    }
                    else
                    {
                        m_visualizedVolumeTypes.emplace_back( pVolumeTypeInfo );
                    }
                }
            }

            ImGui::EndCombo();
        }
        ImGuiX::ItemTooltip( "Volume Visualizations" );
        ImGui::PopStyleVar();
    }

    void EntityEditorBaseWorkspace::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        auto drawingCtx = GetDrawingContext();
        auto pEditedMap = m_context.GetMap();

        // Overlay Widgets
        //-------------------------------------------------------------------------

        auto DrawComponentWorldIcon = [this, pViewport] ( SpatialEntityComponent* pComponent, char icon[4], bool isSelected )
        {
            if ( pViewport->IsWorldSpacePointVisible( pComponent->GetPosition() ) )
            {
                ImVec2 const positionScreenSpace = pViewport->WorldSpaceToScreenSpace( pComponent->GetPosition() );
                if ( ImGuiX::DrawOverlayIcon( positionScreenSpace, icon, pComponent, isSelected, ImColor( Colors::Yellow.ToFloat4() ) ) )
                {
                    return true;
                }
            }

            return false;
        };

        auto const& registeredLights = m_pWorld->GetAllRegisteredComponentsOfType<Render::LightComponent>();
        for ( auto pComponent : registeredLights )
        {
            auto pLightComponent = const_cast<Render::LightComponent*>( pComponent );
            bool const isSelected = m_context.HasSingleSelectedComponent() && m_context.IsSelected( pComponent );
            if ( DrawComponentWorldIcon( pLightComponent, KRG_ICON_LIGHTBULB, isSelected ) )
            {
                auto pEntity = pEditedMap->FindEntity( pLightComponent->GetEntityID() );
                m_context.SelectEntity( pEntity );
                m_context.SelectComponent( pLightComponent );
            }
        }

        auto const& registeredPlayerSpawns = m_pWorld->GetAllRegisteredComponentsOfType<Player::PlayerSpawnComponent>();
        for ( auto pComponent : registeredPlayerSpawns )
        {
            auto pSpawnComponent = const_cast<Player::PlayerSpawnComponent*>( pComponent );
            bool const isSelected = m_context.HasSingleSelectedComponent() && m_context.IsSelected( pComponent );
            if ( DrawComponentWorldIcon( pSpawnComponent, KRG_ICON_GAMEPAD, isSelected ) )
            {
                auto pEntity = pEditedMap->FindEntity( pSpawnComponent->GetEntityID() );
                m_context.SelectEntity( pEntity );
                m_context.SelectComponent( pSpawnComponent );
            }

            auto const& spawnTransform = pComponent->GetWorldTransform();
            drawingCtx.DrawArrow( spawnTransform.GetTranslation(), spawnTransform.GetForwardVector(), 0.5f, Colors::Yellow, 3.0f );
        }

        auto const& registeredAISpawns = m_pWorld->GetAllRegisteredComponentsOfType<AI::AISpawnComponent>();
        for ( auto pComponent : registeredAISpawns )
        {
            auto pSpawnComponent = const_cast<AI::AISpawnComponent*>( pComponent );
            bool const isSelected = m_context.HasSingleSelectedComponent() && m_context.IsSelected( pComponent );
            if ( DrawComponentWorldIcon( pSpawnComponent, KRG_ICON_USER_SECRET, isSelected ) )
            {
                auto pEntity = pEditedMap->FindEntity( pSpawnComponent->GetEntityID() );
                m_context.SelectEntity( pEntity );
                m_context.SelectComponent( pSpawnComponent );
            }

            auto const& spawnTransform = pComponent->GetWorldTransform();
            drawingCtx.DrawArrow( spawnTransform.GetTranslation(), spawnTransform.GetForwardVector(), 0.5f, Colors::Yellow, 3.0f );
        }

        // Draw light debug widgets
        //-------------------------------------------------------------------------
        // TODO: generalized component visualizers

        if ( m_context.HasSingleSelectedComponent() )
        {
            auto pSelectedComponent = m_context.GetSelectedComponent();
            if ( IsOfType<Render::DirectionalLightComponent>( pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::DirectionalLightComponent>( pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + forwardDir, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::SpotLightComponent>( pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::SpotLightComponent>( pSelectedComponent );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->GetLightInnerUmbraAngle(), 1.5f, pLightComponent->GetLightColor(), 3.0f );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->GetLightOuterUmbraAngle(), 1.5f, pLightComponent->GetLightColor(), 3.0f );
            }
            else if ( IsOfType<Render::PointLightComponent>( pSelectedComponent ) )
            {
                auto pLightComponent = Cast<Render::PointLightComponent>( pSelectedComponent );
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

        // Volume Debug
        //-------------------------------------------------------------------------

        for ( auto pVisualizedVolumeTypeInfo : m_visualizedVolumeTypes )
        {
            auto const& volumeComponents = m_pWorld->GetAllRegisteredComponentsOfType( pVisualizedVolumeTypeInfo->m_ID );
            for ( auto pComponent : volumeComponents )
            {
                auto pVolumeComponent = static_cast<VolumeComponent const*>( pComponent );
                pVolumeComponent->Draw( drawingCtx );
            }
        }

        // Selection and Manipulation
        //-------------------------------------------------------------------------

        if ( m_context.HasSpatialSelection() )
        {
            // Draw selection bounds
            //-------------------------------------------------------------------------

            for ( auto pEntity : m_context.GetSelectedEntities() )
            {
                drawingCtx.DrawWireBox( pEntity->GetCombinedWorldBounds(), Colors::Yellow, 3.0f, Drawing::EnableDepthTest );
            }

            drawingCtx.DrawWireBox( m_context.GetSelectionBounds(), Colors::Yellow, 3.0f, Drawing::EnableDepthTest );

            // Update Gizmo
            //-------------------------------------------------------------------------

            if ( m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::None )
            {
                m_gizmo.SwitchMode( ImGuiX::Gizmo::GizmoMode::Translation );
            }

            m_gizmoTransform = m_context.GetSelectionTransform();

            switch ( m_gizmo.Draw( *m_pWorld->GetViewport() ) )
            {
                case ImGuiX::Gizmo::Result::StartedManipulating:
                {
                    bool const duplicateSelection = m_gizmo.GetMode() == ImGuiX::Gizmo::GizmoMode::Translation && ImGui::GetIO().KeyAlt;
                    m_context.BeginTransformManipulation( m_gizmoTransform, duplicateSelection );
                }
                break;

                case ImGuiX::Gizmo::Result::Manipulating:
                {
                    m_context.ApplyTransformManipulation( m_gizmoTransform );
                }
                break;

                case ImGuiX::Gizmo::Result::StoppedManipulating:
                {
                    m_context.EndTransformManipulation( m_gizmoTransform );
                }
                break;
            }
        }
    }

    void EntityEditorBaseWorkspace::UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        m_context.Update( context );

        //-------------------------------------------------------------------------

        if ( m_isViewportFocused || m_isViewportHovered )
        {
            if ( ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Space ) ) )
            {
                m_gizmo.SwitchToNextMode();
            }

            if ( ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Delete ) ) )
            {
                m_context.DestroySelectedEntities();
            }
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_outlinerWindowName.c_str() ) )
        {
            m_entityOutliner.Draw( context );
        }
        ImGui::End();

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_entityViewWindowName.c_str() ) )
        {
            m_entityInspector.Draw( context );
        }
        ImGui::End();

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_propertyGridWindowName.c_str() ) )
        {
            m_propertyGrid.Draw( context );
        }
        ImGui::End();
    }
}