#include "Workspace_MapEditor.h"
#include "Tools/Entity/Serialization/EntityCollectionDescriptorWriter.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/Helpers/CommonDialogs.h"
#include "Tools/Core/Widgets/TreeListView.h"
#include "Game/Core/AI/Components/Component_AISpawn.h"
#include "Engine/Render/Components/Component_Lights.h"
#include "Engine/Core/Components/Component_PlayerSpawn.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Math/Random.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    using namespace TypeSystem;

    //-------------------------------------------------------------------------

    class TypeSelector final
    {
    public:

        enum Mode
        {
            SystemSelector,
            ComponentSelector
        };

    public:

        TypeSelector( TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
            , m_allSystemTypes( typeRegistry.GetAllDerivedTypes( EntitySystem::GetStaticTypeID(), false, false ) )
            , m_allComponentTypes( typeRegistry.GetAllDerivedTypes( EntityComponent::GetStaticTypeID(), false, false ) )
        {
            Memory::MemsetZero( m_filterBuffer, 256 * sizeof( char ) );
        }

        void Initialize( Entity* pEntity )
        {
            KRG_ASSERT( pEntity != nullptr );

            m_systemOptions.clear();
            for ( auto pSystemTypeInfo : m_allSystemTypes )
            {
                bool isValidOption = true;
                for ( auto pExistingSystem : pEntity->GetSystems() )
                {
                    if ( m_typeRegistry.AreTypesInTheSameHierarchy( pExistingSystem->GetTypeInfo(), pSystemTypeInfo ) )
                    {
                        isValidOption = false;
                        break;
                    }
                }

                if ( isValidOption )
                {
                    m_systemOptions.emplace_back( pSystemTypeInfo );
                }
            }

            //-------------------------------------------------------------------------

            m_componentOptions.clear();
            for ( auto pComponentTypeInfo : m_allComponentTypes )
            {
                bool isValidOption = true;
                for ( auto pExistingComponent : pEntity->GetComponents() )
                {
                    if ( pExistingComponent->IsSingletonComponent() )
                    {
                        if ( m_typeRegistry.AreTypesInTheSameHierarchy( pExistingComponent->GetTypeInfo(), pComponentTypeInfo ) )
                        {
                            isValidOption = false;
                            break;
                        }
                    }
                }

                if ( isValidOption )
                {
                    m_componentOptions.emplace_back( pComponentTypeInfo );
                }
            }

            //-------------------------------------------------------------------------

            m_initializeFocus = true;
        }

        // Draws the selector - returns true if the value is changed
        bool DrawPicker( Mode mode )
        {
            ImVec2 const contentRegionAvailable = ImGui::GetContentRegionAvail();

            // Draw Filter
            //-------------------------------------------------------------------------

            bool filterUpdated = false;

            ImGui::SetNextItemWidth( contentRegionAvailable.x - ImGui::GetStyle().WindowPadding.x - 22 );
            InlineString filterCopy( m_filterBuffer );

            if ( m_initializeFocus )
            {
                ImGui::SetKeyboardFocusHere();
                m_initializeFocus = false;
                filterUpdated = true;
            }

            if ( ImGui::InputText( "##Filter", filterCopy.data(), 256 ) )
            {
                if ( strcmp( filterCopy.data(), m_filterBuffer ) != 0 )
                {
                    strcpy_s( m_filterBuffer, 256, filterCopy.data() );

                    // Convert buffer to lower case
                    int32 i = 0;
                    while ( i < 256 && m_filterBuffer[i] != 0 )
                    {
                        m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                        i++;
                    }

                    filterUpdated = true;
                }
            }

            ImGui::SameLine();
            if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear Filter", ImVec2( 22, 0 ) ) )
            {
                m_filterBuffer[0] = 0;
                filterUpdated = true;
            }

            // Update filter options
            //-------------------------------------------------------------------------

            if ( filterUpdated )
            {
                RefreshFilteredOptions( mode );
            }

            // Draw results
            //-------------------------------------------------------------------------

            bool selectionMade = false;
            float const tableHeight = contentRegionAvailable.y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
            if ( ImGui::BeginTable( "Resource List", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2( contentRegionAvailable.x, tableHeight ) ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthStretch, 1.0f );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                clipper.Begin( (int32) m_filteredOptions.size() );
                while ( clipper.Step() )
                {
                    for ( int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++ )
                    {
                        ImGui::TableNextRow();

                        ImGui::TableNextColumn();
                        bool isSelected = ( m_pSelectedType == m_filteredOptions[i] );
                        if ( ImGui::Selectable( m_filteredOptions[i]->GetTypeName(), &isSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns ) )
                        {
                            m_pSelectedType = m_filteredOptions[i];

                            if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                            {
                                selectionMade = true;
                            }
                        }
                    }
                }

                ImGui::EndTable();
            }

            //-------------------------------------------------------------------------

            return selectionMade;
        }

        // Get the selected type - Only use if DrawPicker returns true
        inline TypeInfo const* GetSelectedType() const { return m_pSelectedType; }

    private:

        void RefreshFilteredOptions( Mode mode )
        {
            auto& allOptions = ( mode == ComponentSelector ) ? m_componentOptions : m_systemOptions;

            if ( m_filterBuffer[0] == 0 )
            {
                m_filteredOptions = allOptions;
            }
            else
            {
                m_filteredOptions.clear();
                for ( auto const& pTypeInfo : allOptions )
                {
                    String lowercasePath( pTypeInfo->GetTypeName() );
                    lowercasePath.make_lower();

                    bool passesFilter = true;
                    char* token = strtok( m_filterBuffer, " " );
                    while ( token )
                    {
                        if ( lowercasePath.find( token ) == String::npos )
                        {
                            passesFilter = false;
                            break;
                        }

                        token = strtok( NULL, " " );
                    }

                    if ( passesFilter )
                    {
                        m_filteredOptions.emplace_back( pTypeInfo );
                    }
                }
            }
        }

    private:

        TypeRegistry const&                 m_typeRegistry;
        char                                m_filterBuffer[256];

        TVector<TypeInfo const*> const      m_allSystemTypes;
        TVector<TypeInfo const*> const      m_allComponentTypes;
        TVector<TypeInfo const*>            m_systemOptions;
        TVector<TypeInfo const*>            m_componentOptions;
        TVector<TypeInfo const*>            m_filteredOptions;

        TypeInfo const*                     m_pSelectedType = nullptr;
        TInlineVector<TypeID, 10>           m_excludedTypes;
        bool                                m_initializeFocus = false;
    };
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityInternalItem final : public TreeListViewItem
    {

    public:

        EntityInternalItem( char const* pLabel )
            : TreeListViewItem()
            , m_nameID( pLabel )
        {}

        EntityInternalItem( IRegisteredType* pTypeInstance )
            : TreeListViewItem()
            , m_pInstance( pTypeInstance )
        {}

        inline IRegisteredType* GetTypeInstance() { return m_pInstance; }

        virtual StringID GetNameID() const override
        {
            if ( IsCategoryLabel() )
            {
                return m_nameID;
            }
            else if ( IsEntity() )
            {
                return Cast<Entity>( m_pInstance )->GetName();
            }
            else if ( IsComponent() )
            {
                return Cast<EntityComponent>( m_pInstance )->GetName();
            }
            else
            {
                return Cast<EntitySystem>( m_pInstance )->GetTypeID().ToStringID();
            }
        }

        virtual uint64 GetUniqueID() const override
        {
            if ( IsCategoryLabel() )
            {
                return m_nameID.GetID();
            }
            else if ( IsEntity() )
            {
                return Cast<Entity>( m_pInstance )->GetID().ToUint64();
            }
            else if ( IsComponent() )
            {
                return Cast<EntityComponent>( m_pInstance )->GetID().ToUint64();
            }
            else
            {
                return Cast<EntitySystem>( m_pInstance )->GetTypeID().ToStringID().GetID();
            }
        }

        virtual bool HasContextMenu() const override { return !IsCategoryLabel(); }

        inline bool IsCategoryLabel() const { return m_pInstance == nullptr; }
        inline bool IsEntity() const { return m_pInstance != nullptr && IsOfType<Entity>( m_pInstance ); }
        inline bool IsSystem() const { return m_pInstance != nullptr && IsOfType<EntitySystem>( m_pInstance ); }
        inline bool IsComponent() const { return m_pInstance != nullptr && IsOfType<EntityComponent>( m_pInstance ); }
        inline bool IsSpatialComponent() const { return m_pInstance != nullptr && IsOfType<SpatialEntityComponent>( m_pInstance ); }

    private:

        IRegisteredType*    m_pInstance = nullptr;
        StringID            m_nameID;
    };

    //-------------------------------------------------------------------------

    class EntityInspectorTreeView final : public TreeListView
    {
    public:

        enum Command
        {
            None,
            AddSystem,
            AddComponent,
            Delete
        };

    public:

        EntityInspectorTreeView()
        {
            m_expandItemsOnlyViaArrow = true;
        }

        void SetEntityToInspect( Entity* pEntity )
        {
            m_pEntity = pEntity;
            RebuildTree( false );
        }

        inline Command PopRequestedCommand()
        {
            Command poppedCommand = m_requestedCommand;
            m_requestedCommand = Command::None;
            return poppedCommand;
        }

        void Refresh() { RebuildTree( false ); }

    private:

        virtual void RebuildTreeInternal() override
        {
            if ( m_pEntity == nullptr )
            {
                return;
            }

            // Entity
            //-------------------------------------------------------------------------

            auto pEntityRoot = m_rootItem.CreateChild<EntityInternalItem>( m_pEntity );
            pEntityRoot->SetExpanded( true );

            // Systems
            //-------------------------------------------------------------------------

            if ( !m_pEntity->GetSystems().empty() )
            {
                auto pSystemsRoot = pEntityRoot->CreateChild<EntityInternalItem>( "Systems" );
                pSystemsRoot->SetExpanded( true );
                for ( auto pSystem : m_pEntity->GetSystems() )
                {
                    pSystemsRoot->CreateChild<EntityInternalItem>( pSystem );
                }
            }

            // Components
            //-------------------------------------------------------------------------

            TInlineVector<EntityComponent*, 10> components;
            TInlineVector<SpatialEntityComponent*, 10> spatialComponents;

            for ( auto pComponent : m_pEntity->GetComponents() )
            {
                if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
                {
                    spatialComponents.emplace_back( pSpatialComponent );
                }
                else
                {
                    components.emplace_back( pComponent );
                }
            }

            if ( !components.empty() )
            {
                auto pComponentsRoot = pEntityRoot->CreateChild<EntityInternalItem>( "Components" );
                pComponentsRoot->SetExpanded( true );
                for ( auto pComponent : components )
                {
                    pComponentsRoot->CreateChild<EntityInternalItem>( pComponent );
                }
            }

            if ( !spatialComponents.empty() )
            {
                auto pComponentsRoot = pEntityRoot->CreateChild<EntityInternalItem>( "Spatial Components" );
                pComponentsRoot->SetExpanded( true );
                for ( auto pComponent : spatialComponents )
                {
                    pComponentsRoot->CreateChild<EntityInternalItem>( pComponent );
                }
            }
        }

        virtual uint32 GetNumExtraColumns() const override
        { 
            return 1;
        }

        virtual void SetupExtraColumnHeaders() const override
        {
            ImGui::TableSetupColumn( "Details", ImGuiTableColumnFlags_WidthFixed, 200 );
        }

        virtual void DrawItemExtraColumns( TreeListViewItem* pBaseItem, int32 extraColumnIdx ) override
        {
            KRG_ASSERT( extraColumnIdx == 0 );

            ImGuiX::ScopedFont sf( ImGuiX::Font::Tiny );
            auto pItem = static_cast<EntityInternalItem*>( pBaseItem );
            if ( pItem->IsComponent() )
            {
                TInlineString<100> typeName( pItem->GetTypeInstance()->GetTypeInfo()->GetTypeName() + 5 );
                ImGui::Text( typeName.c_str() );
            }
        }

        virtual void DrawItemContextMenu( TreeListViewItem* pBaseItem )
        {
            auto pItem = static_cast<EntityInternalItem*>( pBaseItem );
            KRG_ASSERT( !pItem->IsCategoryLabel() );

            //-------------------------------------------------------------------------

            if ( pItem->IsEntity() )
            {
                if ( ImGui::MenuItem( "Add System" ) )
                {
                    m_requestedCommand = AddSystem;
                }

                if ( ImGui::MenuItem( "Add Component" ) )
                {
                    m_requestedCommand = AddComponent;
                }
            }

            //-------------------------------------------------------------------------

            if ( pItem->IsSpatialComponent() )
            {
                if ( ImGui::MenuItem( "Add Child Component" ) )
                {
                    m_requestedCommand = AddComponent;
                }
            }

            //-------------------------------------------------------------------------

            if ( pItem->IsSystem() || pItem->IsComponent() )
            {
                if ( ImGui::MenuItem( "Delete" ) )
                {
                    m_requestedCommand = Delete;
                }
            }
        }

    private:

        Entity*                         m_pEntity = nullptr;
        Command                         m_requestedCommand = Command::None;
    };
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapEditor::EntityMapEditor( WorkspaceInitializationContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
        , m_pEntityInspectorTreeView( KRG::New<EntityInspectorTreeView>() )
        , m_pTypeSelector( KRG::New<TypeSelector>( *context.m_pTypeRegistry ) )
        , m_propertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        m_gizmo.SetTargetTransform( &m_editedTransform );

        m_entityStateChangedBindingID = Entity::OnEntityUpdated().Bind( [this] ( Entity* pEntityChanged ) { OnEntityStateChanged( pEntityChanged ); } );
        m_preEditBindingID = m_propertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PreEdit( eventInfo ); } );
        m_postEditBindingID = m_propertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& eventInfo ) { PostEdit( eventInfo ); } );
        m_inspectorSelectionChangedBindingID = m_pEntityInspectorTreeView->OnSelectedChanged().Bind( [this] () { OnInspectorSelectionChanged(); } );

        SetDisplayName( "Map Editor" );
    }

    EntityMapEditor::~EntityMapEditor()
    {
        m_propertyGrid.OnPreEdit().Unbind( m_preEditBindingID );
        m_propertyGrid.OnPostEdit().Unbind( m_postEditBindingID );
        Entity::OnEntityUpdated().Unbind( m_entityStateChangedBindingID );

        m_pEntityInspectorTreeView->OnSelectedChanged().Unbind( m_inspectorSelectionChangedBindingID );
        KRG::Delete( m_pEntityInspectorTreeView );

        KRG::Delete( m_pTypeSelector );
    }

    void EntityMapEditor::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );

        m_outlinerWindowName.sprintf( "Map Outliner", GetID() );
        m_entityViewWindowName.sprintf( "Entity Inspector##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::CreateNewMap()
    {
        // Should we save the current map before unloading?
        if ( IsDirty() )
        {
            pfd::message saveChangesMsg( m_loadedMap.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
            if ( saveChangesMsg.result() == pfd::button::yes )
            {
                Save();
            }
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        auto const mapFilename = pfd::save_file( "Create New Map", m_pResourceDatabase->GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" } ).result();
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

        ResourceID const mapResourceID = GetResourcePath( mapFilePath );
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
        if ( writer.WriteCollection( *m_pTypeRegistry, mapFilePath, emptyMap ) )
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
        auto const selectedMap = pfd::open_file( "Load Map", m_pResourceDatabase->GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
        if ( selectedMap.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( selectedMap[0].c_str() );
        ResourceID const mapToLoad = GetResourcePath( mapFilePath );
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
                pfd::message saveChangesMsg( m_loadedMap.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no);
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
            SetDisplayName( m_loadedMap.GetResourcePath().GetFileNameWithoutExtension() );
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

        FileSystem::Path const mapFilePath = SaveDialog( "Map", GetFileSystemPath( m_loadedMap ).GetParentDirectory().c_str(), "Map File");
        if ( !mapFilePath.IsValid() )
        {
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptorWriter writer;
        if ( writer.WriteCollection( *m_pTypeRegistry, mapFilePath, *pEditedMap ) )
        {
            ResourceID const mapResourcePath = GetResourcePath( mapFilePath );
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

        FileSystem::Path const filePath = GetFileSystemPath( m_loadedMap );
        EntityCollectionDescriptorWriter writer;
        return writer.WriteCollection( *m_pTypeRegistry, filePath, *pEditedMap );
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
        if ( m_pSelectedEntity != pEntity )
        {
            m_pSelectedEntity = pEntity;
            m_pSelectedComponent = nullptr;
            m_pEntityInspectorTreeView->SetEntityToInspect( pEntity );
            m_propertyGrid.SetTypeToEdit( pEntity );
            m_syncOutlinerToSelectedItem = true;
        }
    }

    void EntityMapEditor::SelectComponent( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pSelectedEntity != nullptr && pComponent != nullptr );
        KRG_ASSERT( m_pSelectedEntity->GetID() == pComponent->GetEntityID() );
        if ( m_pSelectedComponent != pComponent )
        {
            m_pSelectedComponent = pComponent;
            m_propertyGrid.SetTypeToEdit( pComponent );
        }
    }

    void EntityMapEditor::ClearSelection()
    {
        m_pSelectedEntity = nullptr;
        m_pSelectedComponent = nullptr;
        m_propertyGrid.SetTypeToEdit( nullptr );
        m_pEntityInspectorTreeView->SetEntityToInspect( nullptr );
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
        if ( ImGui::BeginMenu( KRG_ICON_GLOBE" Map" ) )
        {
            if ( ImGui::MenuItem( "Create New Map" ) )
            {
                CreateNewMap();
            }

            if ( ImGui::MenuItem( "Load Map" ) )
            {
                SelectAndLoadMap();
            }

            if ( ImGui::MenuItem( "Save Map As" ) )
            {
                SaveMapAs();
            }

            ImGui::EndMenu();
        }

        DrawDefaultToolbarItems();
    }

    void EntityMapEditor::DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        HandleInput( context );

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        DrawMapOutliner( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawEntityInspector( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawPropertyGrid( context );
    }

    void EntityMapEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( BeginViewportToolbarGroup( "SpatialControls", ImVec2(80, 22 ) ) )
        {
            TInlineString<10> const coordinateSpaceSwitcherLabel( TInlineString<10>::CtorSprintf(), "%s##CoordinateSpace", m_gizmo.IsInWorldSpace() ? KRG_ICON_GLOBE : KRG_ICON_DOT_CIRCLE_O );
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
                ImGui::BeginDisabled( !HasLoadedMap() );
                if ( ImGuiX::ColoredButton( Colors::Green, Colors::White, "CREATE ENTITY", ImVec2( -1, 0 ) ) )
                {
                    InlineString entityName( "New Entity" );
                    StringID entityNameID( entityName.c_str() );
                    for ( auto pEntity : m_pWorld->GetMap( m_loadedMap )->GetEntities() )
                    {
                        if ( pEntity->GetName() == entityNameID )
                        {
                            entityName.sprintf( "New Entity %u", (uint32) Math::GetRandomUInt() );
                            entityNameID = StringID( entityName.c_str() );
                            break;
                        }
                    }

                    Entity* pEntity = KRG::New<Entity>( entityNameID );
                    m_pWorld->GetMap( m_loadedMap )->AddEntity( pEntity );
                    SelectEntity( pEntity );
                }
                ImGui::EndDisabled();
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
// PropertyGrid
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityMapEditor::DrawPropertyGrid( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_propertyGridWindowName.c_str() ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }

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
}

//-------------------------------------------------------------------------
// Entity Inspector
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityMapEditor::DrawEntityInspector( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_entityViewWindowName.c_str() ) )
        {
            if ( m_pSelectedEntity != nullptr )
            {
                // Entity Details
                //-------------------------------------------------------------------------

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
                    constexpr static float const padding = 20;
                    ImGui::NewLine();
                    ImGui::SameLine( padding, 0 );
                    ImGuiX::DisplayTransform( m_pSelectedEntity->GetWorldTransform(), ImGui::GetContentRegionAvail().x - padding );
                }

                //-------------------------------------------------------------------------

                m_pEntityInspectorTreeView->Draw();
                auto requestedCommand = m_pEntityInspectorTreeView->PopRequestedCommand();

                switch ( requestedCommand )
                {
                    case EntityInspectorTreeView::AddComponent:
                    {
                        TInlineVector<TypeSystem::TypeID, 10> restrictions;
                        for ( auto pComponent : m_pSelectedEntity->GetComponents() )
                        {
                            if ( pComponent->IsSingletonComponent() )
                            {
                                restrictions.emplace_back( pComponent->GetTypeID() );
                            }
                        }

                        m_pTypeSelector->Initialize( m_pSelectedEntity );
                        ImGui::OpenPopup( s_addComponentDialogTitle );
                    }
                    break;

                    case EntityInspectorTreeView::AddSystem:
                    {
                        TInlineVector<TypeSystem::TypeID, 10> restrictions;
                        for ( auto pSystem : m_pSelectedEntity->GetSystems() )
                        {
                            restrictions.emplace_back( pSystem->GetTypeID() );
                        }

                        m_pTypeSelector->Initialize( m_pSelectedEntity );
                        ImGui::OpenPopup( s_addSystemDialogTitle );
                    }
                    break;

                    case EntityInspectorTreeView::Delete:
                    {
                        auto pItem = static_cast<EntityInternalItem*>( m_pEntityInspectorTreeView->GetSelectedItem() );
                        if ( pItem->IsSystem() )
                        {
                            m_pSelectedEntity->DestroySystem( pItem->GetTypeInstance()->GetTypeID() );
                        }
                        else if ( pItem->IsComponent() )
                        {
                            m_pSelectedEntity->DestroyComponent( Cast<EntityComponent>( pItem->GetTypeInstance() )->GetID() );
                        }

                        m_pEntityInspectorTreeView->ClearSelection();
                        m_pEntityInspectorTreeView->ClearActiveItem();
                    }
                    break;
                }

                //-------------------------------------------------------------------------

                DrawAddComponentDialog();
                DrawAddSystemDialog();
            }
        }
        ImGui::End();
    }

    void EntityMapEditor::OnInspectorSelectionChanged()
    {
        auto pItem = static_cast<EntityInternalItem*>( m_pEntityInspectorTreeView->GetSelectedItem() );
        if ( pItem == nullptr || pItem->IsCategoryLabel() || pItem->IsEntity() )
        {
            SelectEntity( m_pSelectedEntity );
        }
        else if ( pItem->IsComponent() )
        {
            SelectComponent( Cast<EntityComponent>( pItem->GetTypeInstance() ) );
        }
    }

    void EntityMapEditor::OnEntityStateChanged( Entity* pEntityChanged )
    {
        if ( pEntityChanged == m_pSelectedEntity )
        {
            m_pEntityInspectorTreeView->Refresh();
        }
    }

    void EntityMapEditor::DrawAddSystemDialog()
    {
        TypeSystem::TypeInfo const* pSystemTypeInfo = nullptr;
        bool isOpen = true;

        ImGui::SetNextWindowSize( ImVec2( 1000, 400 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 400, 400 ), ImVec2( FLT_MAX, FLT_MAX ) );
        if ( ImGui::BeginPopupModal( s_addSystemDialogTitle, &isOpen ) )
        {
            if ( m_pTypeSelector->DrawPicker( TypeSelector::SystemSelector ) )
            {
                pSystemTypeInfo = m_pTypeSelector->GetSelectedType();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        //-------------------------------------------------------------------------

        if ( pSystemTypeInfo != nullptr )
        {
            m_pSelectedEntity->CreateSystem( pSystemTypeInfo );
        }
    }

    void EntityMapEditor::DrawAddComponentDialog()
    {
        TypeSystem::TypeInfo const* pComponentTypeInfo = nullptr;
        bool isOpen = true;

        ImGui::SetNextWindowSize( ImVec2( 1000, 400 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 400, 400 ), ImVec2( FLT_MAX, FLT_MAX ) );
        if ( ImGui::BeginPopupModal( s_addComponentDialogTitle, &isOpen ) )
        {
            if ( m_pTypeSelector->DrawPicker( TypeSelector::ComponentSelector ) )
            {
                pComponentTypeInfo = m_pTypeSelector->GetSelectedType();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        //-------------------------------------------------------------------------

        if ( pComponentTypeInfo != nullptr )
        {
            auto pParentSpatialComponent = TryCast<SpatialEntityComponent>( m_pSelectedComponent );
            ComponentID parentComponentID = ( pParentSpatialComponent != nullptr ) ? pParentSpatialComponent->GetID() : ComponentID();
            m_pSelectedEntity->CreateComponent( pComponentTypeInfo, parentComponentID );
        }
    }
}