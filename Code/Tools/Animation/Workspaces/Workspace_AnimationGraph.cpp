#include "Workspace_AnimationGraph.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Definition.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"
#include "Engine/Animation/Systems/EntitySystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Render/Components/Component_SkeletalMesh.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationGraphEditorFactory, AnimationGraphDefinition, AnimationGraphWorkspace );

    //-------------------------------------------------------------------------

    static InlineString<255> GenerateFilePathForVariation( FileSystem::Path const& graphPath, StringID variationID )
    {
        FileSystem::Path const parentDirectory = graphPath.GetParentDirectory();
        String const filenameNoExtension = graphPath.GetFileNameWithoutExtension();

        InlineString<255> variationPathStr;
        variationPathStr.sprintf( "%s%s_%s.agv", parentDirectory.c_str(), filenameNoExtension.c_str(), variationID.c_str() );
        return variationPathStr;
    }

    //-------------------------------------------------------------------------

    class GraphUndoableAction final : public IUndoableAction
    {
    public:

        GraphUndoableAction( TypeSystem::TypeRegistry const& typeRegistry, AnimationGraphEditorDefinition* pEditorGraph )
            : m_typeRegistry( typeRegistry )
            , m_pGraphDefinition( pEditorGraph )
        {
            KRG_ASSERT( m_pGraphDefinition != nullptr );
        }

        virtual void Undo() override
        {
            JsonReader reader;
            reader.ReadFromString( m_valueBefore.c_str() );
            m_pGraphDefinition->LoadFromJson( m_typeRegistry, reader.GetDocument() );
        }

        virtual void Redo() override
        {
            JsonReader reader;
            reader.ReadFromString( m_valueAfter.c_str() );
            m_pGraphDefinition->LoadFromJson( m_typeRegistry, reader.GetDocument() );
        }

        void SerializeBeforeState()
        {
            JsonWriter writer;
            m_pGraphDefinition->SaveToJson( m_typeRegistry, *writer.GetWriter() );
            m_valueBefore.resize( writer.GetStringBuffer().GetSize() );
            memcpy( m_valueBefore.data(), writer.GetStringBuffer().GetString(), writer.GetStringBuffer().GetSize() );
        }

        void SerializeAfterState()
        {
            JsonWriter writer;
            m_pGraphDefinition->SaveToJson( m_typeRegistry, *writer.GetWriter() );
            m_valueAfter.resize( writer.GetStringBuffer().GetSize() );
            memcpy( m_valueAfter.data(), writer.GetStringBuffer().GetString(), writer.GetStringBuffer().GetSize() );
        }

    private:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        AnimationGraphEditorDefinition*      m_pGraphDefinition = nullptr;
        String                              m_valueBefore;
        String                              m_valueAfter;
    };

    //-------------------------------------------------------------------------

    AnimationGraphWorkspace::AnimationGraphWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<AnimationGraphDefinition>( context, pWorld, resourceID, false )
        , m_propertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        // Load graph from descriptor
        //-------------------------------------------------------------------------

        m_graphFilePath = GetFileSystemPath( resourceID.GetResourcePath() );
        if ( m_graphFilePath.IsValid() )
        {
            bool graphLoadFailed = false;

            // Try read JSON data
            JsonReader reader;
            if ( !reader.ReadFromFile( m_graphFilePath ) )
            {
                graphLoadFailed = true;
            }

            // Try to load the graph from the file
            m_pGraphDefinition = KRG::New<AnimationGraphEditorDefinition>();
            graphLoadFailed = !m_pGraphDefinition->LoadFromJson( *m_pTypeRegistry, reader.GetDocument() );

            // Load failed, so clean up and create a new graph
            if ( graphLoadFailed )
            {
                KRG_LOG_ERROR( "Animation", "Failed to load graph definition: %s", m_graphFilePath.c_str() );
                KRG::Delete( m_pGraphDefinition );
                m_pGraphDefinition = KRG::New<AnimationGraphEditorDefinition>();
                m_pGraphDefinition->CreateNew();
            }
        }

        KRG_ASSERT( m_pGraphDefinition != nullptr );

        // Create editors
        //-------------------------------------------------------------------------

        m_pControlParameterEditor = KRG::New<GraphControlParameterEditor>( m_pGraphDefinition );
        m_pVariationEditor = KRG::New<GraphVariationEditor>( m_pResourceDatabase, m_pGraphDefinition );
        m_pGraphEditor = KRG::New<GraphEditor>( *m_pTypeRegistry, m_pGraphDefinition );

        // Bind events
        //-------------------------------------------------------------------------

        auto OnBeginGraphModification = [this] ( VisualGraph::BaseGraph* pRootGraph )
        {
            if ( pRootGraph == m_pGraphDefinition->GetRootGraph() )
            {
                KRG_ASSERT( m_pActiveUndoableAction == nullptr );

                m_pActiveUndoableAction = KRG::New<GraphUndoableAction>( *m_pTypeRegistry, m_pGraphDefinition );
                m_pActiveUndoableAction->SerializeBeforeState();
            }
        };

        auto OnEndGraphModification = [this] ( VisualGraph::BaseGraph* pRootGraph )
        {
            if ( pRootGraph == m_pGraphDefinition->GetRootGraph() )
            {
                KRG_ASSERT( m_pActiveUndoableAction != nullptr );

                m_pActiveUndoableAction->SerializeAfterState();
                m_undoStack.RegisterAction( m_pActiveUndoableAction );
                m_pActiveUndoableAction = nullptr;
                m_pGraphDefinition->MarkDirty();
            }
        };

        m_rootGraphBeginModificationBindingID = VisualGraph::BaseGraph::OnBeginModification().Bind( OnBeginGraphModification );
        m_rootGraphEndModificationBindingID = VisualGraph::BaseGraph::OnEndModification().Bind( OnEndGraphModification );

        //-------------------------------------------------------------------------

        m_preEditEventBindingID = m_propertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& info ) { m_pGraphDefinition->GetRootGraph()->BeginModification(); } );
        m_postEditEventBindingID = m_propertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& info ) { m_pGraphDefinition->GetRootGraph()->EndModification(); } );
    }

    AnimationGraphWorkspace::~AnimationGraphWorkspace()
    {
        if ( IsPreviewing() )
        {
            StopPreview();
        }

        KRG::Delete( m_pGraphDefinition );

        KRG::Delete( m_pControlParameterEditor );
        KRG::Delete( m_pVariationEditor );
        KRG::Delete( m_pGraphEditor );

        m_propertyGrid.OnPreEdit().Unbind( m_preEditEventBindingID );
        m_propertyGrid.OnPostEdit().Unbind( m_postEditEventBindingID );

        VisualGraph::BaseGraph::OnBeginModification().Unbind( m_rootGraphBeginModificationBindingID );
        VisualGraph::BaseGraph::OnEndModification().Unbind( m_rootGraphEndModificationBindingID );
    }

    void AnimationGraphWorkspace::Initialize( UpdateContext const& context )
    {
        TResourceWorkspace<AnimationGraphDefinition>::Initialize( context );

        m_controlParametersWindowName.sprintf( "Control Parameters##%u", GetID() );
        m_graphViewWindowName.sprintf( "Graph View##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
        m_variationEditorWindowName.sprintf( "Variation Editor##%u", GetID() );
    }

    void AnimationGraphWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topLeftDockID = 0, bottomLeftDockID = 0, centerDockID = 0, rightDockID = 0;

        ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.2f, &topLeftDockID, &centerDockID );
        ImGui::DockBuilderSplitNode( topLeftDockID, ImGuiDir_Down, 0.33f, &bottomLeftDockID, &topLeftDockID );
        ImGui::DockBuilderSplitNode( centerDockID, ImGuiDir_Left, 0.66f, &centerDockID, &rightDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), rightDockID );
        ImGui::DockBuilderDockWindow( m_controlParametersWindowName.c_str(), topLeftDockID );
        ImGui::DockBuilderDockWindow( m_propertyGridWindowName.c_str(), bottomLeftDockID );
        ImGui::DockBuilderDockWindow( m_graphViewWindowName.c_str(), centerDockID );
        ImGui::DockBuilderDockWindow( m_variationEditorWindowName.c_str(), centerDockID );
    }

    void AnimationGraphWorkspace::DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        DebugContext* pDebugContext = nullptr;
        if ( m_isPreviewing && m_pGraphComponent->IsInitialized() )
        {
            pDebugContext = &m_debugContext;
        }

        //-------------------------------------------------------------------------

        m_pControlParameterEditor->UpdateAndDraw( context, pDebugContext, pWindowClass, m_controlParametersWindowName.c_str() );
        m_pGraphEditor->UpdateAndDraw( context, pDebugContext, pWindowClass, m_graphViewWindowName.c_str() );
        m_pVariationEditor->UpdateAndDraw( context, pWindowClass, m_variationEditorWindowName.c_str() );

        // Property Grid
        //-------------------------------------------------------------------------

        auto const& selection = m_pGraphEditor->GetSelectedNodes();
        if ( selection.empty() )
        {
            m_propertyGrid.SetTypeToEdit( nullptr );
        }
        else
        {
            m_propertyGrid.SetTypeToEdit( selection.back().m_pNode );
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_propertyGridWindowName.c_str() ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }

    void AnimationGraphWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( IsPreviewing() )
        {
            if ( ImGui::Button( "Stop Preview" ) )
            {
                StopPreview();
            }
        }
        else
        {
            if ( ImGui::Button( "Start Preview" ) )
            {
                StartPreview();
            }
        }
    }

    //-------------------------------------------------------------------------

    void AnimationGraphWorkspace::GenerateAnimGraphVariationDescriptors()
    {
        KRG_ASSERT( m_graphFilePath.IsValid() && m_graphFilePath.MatchesExtension( "ag" ) );

        auto const& variations = m_pGraphDefinition->GetVariationHierarchy();
        for ( auto const& variation : variations.GetAllVariations() )
        {
            AnimationGraphVariationResourceDescriptor resourceDesc;
            resourceDesc.m_graphPath = GetResourcePath( m_graphFilePath );
            resourceDesc.m_variationID = variation.m_ID;

            InlineString<255> const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, variation.m_ID );
            FileSystem::Path const variationPath( variationPathStr.c_str() );

            WriteResourceDescriptorToFile( *m_pTypeRegistry, variationPath, &resourceDesc );
        }
    }

    bool AnimationGraphWorkspace::Save()
    {
        KRG_ASSERT( m_graphFilePath.IsValid() );
        JsonWriter writer;
        m_pGraphDefinition->SaveToJson( *m_pTypeRegistry, *writer.GetWriter() );
        if ( writer.WriteToFile( m_graphFilePath ) )
        {
            GenerateAnimGraphVariationDescriptors();
            m_pGraphDefinition->ClearDirty();
            return true;
        }

        return false;
    }

    bool AnimationGraphWorkspace::IsDirty() const
    {
        return m_pGraphDefinition->IsDirty();
    }

    void AnimationGraphWorkspace::OnUndoRedo()
    {
        m_pGraphEditor->OnUndoRedo();
    }

    //-------------------------------------------------------------------------

    void AnimationGraphWorkspace::StartPreview()
    {
        // Try to compile the graph
        //-------------------------------------------------------------------------

        EditorGraphCompilationContext compilationContext;
        if ( m_pGraphDefinition->Compile( compilationContext ) )
        {
            m_debugContext.m_nodeIDtoIndexMap = compilationContext.GetIDToIndexMap();
        }
        else // Compilation failed, stop preview attempt
        {
            for ( auto const& entry : compilationContext.GetLog() )
            {
                if ( entry.m_severity == Log::Severity::Warning )
                {
                    KRG_LOG_WARNING( "Anim", entry.m_message.c_str() );
                }
                else if ( entry.m_severity == Log::Severity::Error )
                {
                    KRG_LOG_ERROR( "Anim", entry.m_message.c_str() );
                }
            }

            return;
        }

        // Save the graph changes
        //-------------------------------------------------------------------------
        // Ensure that we save the graph and re-generate the dataset on preview

        Save();

        //-------------------------------------------------------------------------

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );

        // Graph Component
        //-------------------------------------------------------------------------

        InlineString<255> const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, m_selectedVariationID );
        ResourceID const graphVariationResourceID( GetResourcePath( variationPathStr.c_str()) );

        m_pGraphComponent = KRG::New<AnimationGraphComponent>( StringID( "Animation Component" ) );
        m_pGraphComponent->SetGraphVariation( graphVariationResourceID );
        m_pPreviewEntity->AddComponent( m_pGraphComponent );

        m_debugContext.m_pGraphComponent = m_pGraphComponent;

        // Preview Mesh Component
        //-------------------------------------------------------------------------

        auto pVariation = m_pGraphDefinition->GetVariation( m_selectedVariationID );
        KRG_ASSERT( pVariation != nullptr );
        if ( pVariation->m_pSkeleton.IsValid() )
        {
            // Load resource descriptor for skeleton to get the preview mesh
            FileSystem::Path const resourceDescPath = GetFileSystemPath( pVariation->m_pSkeleton.GetResourcePath() );
            SkeletonResourceDescriptor resourceDesc;
            if ( TryReadResourceDescriptorFromFile( *m_pTypeRegistry, resourceDescPath, resourceDesc ) )
            {
                // Create a preview mesh component
                auto pMeshComponent = KRG::New<Render::SkeletalMeshComponent>( StringID( "Mesh Component" ) );
                pMeshComponent->SetSkeleton( pVariation->m_pSkeleton.GetResourceID() );
                pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
                m_pPreviewEntity->AddComponent( pMeshComponent );
            }
        }

        // Systems
        //-------------------------------------------------------------------------

        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        // Add the entity
        //-------------------------------------------------------------------------
        
        AddEntityToWorld( m_pPreviewEntity );
        m_isPreviewing = true;
    }

    void AnimationGraphWorkspace::StopPreview()
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );
        DestroyEntityInWorld( m_pPreviewEntity );

        m_pPreviewEntity = nullptr;
        m_pGraphComponent = nullptr;

        m_debugContext.m_pGraphComponent = nullptr;
        m_debugContext.m_nodeIDtoIndexMap.clear();

        m_isPreviewing = false;
    }
}