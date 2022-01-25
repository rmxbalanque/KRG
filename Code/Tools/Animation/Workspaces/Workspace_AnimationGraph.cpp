#include "Workspace_AnimationGraph.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Definition.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"
#include "Engine/Animation/Systems/EntitySystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Render/Components/Component_SkeletalMesh.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Animation/DebugViews/DebugView_Animation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationGraphEditorFactory, GraphDefinition, AnimationGraphWorkspace );

    //-------------------------------------------------------------------------

    static InlineString GenerateFilePathForVariation( FileSystem::Path const& graphPath, StringID variationID )
    {
        FileSystem::Path const parentDirectory = graphPath.GetParentDirectory();
        String const filenameNoExtension = graphPath.GetFileNameWithoutExtension();

        InlineString variationPathStr;
        variationPathStr.sprintf( "%s%s_%s.agv", parentDirectory.c_str(), filenameNoExtension.c_str(), variationID.c_str() );
        return variationPathStr;
    }

    //-------------------------------------------------------------------------

    class GraphUndoableAction final : public IUndoableAction
    {
    public:

        GraphUndoableAction( AnimationGraphWorkspace* pWorkspace, TypeSystem::TypeRegistry const& typeRegistry, EditorGraphDefinition* pEditorGraph )
            : m_typeRegistry( typeRegistry )
            , m_pGraphDefinition( pEditorGraph )
            , m_pWorkspace( pWorkspace )
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
            if ( m_pWorkspace->IsPreviewing() )
            {
                m_pWorkspace->StopPreview();
            }

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

        AnimationGraphWorkspace*            m_pWorkspace = nullptr;
        TypeSystem::TypeRegistry const&     m_typeRegistry;
        EditorGraphDefinition*              m_pGraphDefinition = nullptr;
        String                              m_valueBefore;
        String                              m_valueAfter;
    };

    //-------------------------------------------------------------------------

    AnimationGraphWorkspace::AnimationGraphWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<GraphDefinition>( context, pWorld, resourceID, false )
        , m_propertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        SetViewportCameraSpeed( 10.0f );
        SetWorldTimeControlsEnabled( true );

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
            m_pGraphDefinition = KRG::New<EditorGraphDefinition>();
            graphLoadFailed = !m_pGraphDefinition->LoadFromJson( *m_pTypeRegistry, reader.GetDocument() );

            // Load failed, so clean up and create a new graph
            if ( graphLoadFailed )
            {
                KRG_LOG_ERROR( "Animation", "Failed to load graph definition: %s", m_graphFilePath.c_str() );
                KRG::Delete( m_pGraphDefinition );
                m_pGraphDefinition = KRG::New<EditorGraphDefinition>();
                m_pGraphDefinition->CreateNew();
            }
        }

        KRG_ASSERT( m_pGraphDefinition != nullptr );

        // Create editors
        //-------------------------------------------------------------------------

        m_pControlParameterEditor = KRG::New<GraphControlParameterEditor>( m_pGraphDefinition );
        m_pVariationEditor = KRG::New<GraphVariationEditor>( m_pResourceDatabase, m_pGraphDefinition );
        m_pGraphEditor = KRG::New<GraphEditor>( *m_pTypeRegistry, *context.m_pResourceDatabase, m_pGraphDefinition );

        // Bind events
        //-------------------------------------------------------------------------

        auto OnBeginGraphModification = [this] ( VisualGraph::BaseGraph* pRootGraph )
        {
            if ( pRootGraph == m_pGraphDefinition->GetRootGraph() )
            {
                KRG_ASSERT( m_pActiveUndoableAction == nullptr );

                m_pActiveUndoableAction = KRG::New<GraphUndoableAction>( this, *m_pTypeRegistry, m_pGraphDefinition );
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
        TResourceWorkspace<GraphDefinition>::Initialize( context );

        m_controlParametersWindowName.sprintf( "Control Parameters##%u", GetID() );
        m_graphViewWindowName.sprintf( "Graph View##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
        m_variationEditorWindowName.sprintf( "Variation Editor##%u", GetID() );
        m_debuggerWindowName.sprintf( "Debugger##%u", GetID() );
    }

    void AnimationGraphWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topLeftDockID = 0, bottomLeftDockID = 0, centerDockID = 0, rightDockID = 0, bottomRightDockID;

        ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.2f, &topLeftDockID, &centerDockID );
        ImGui::DockBuilderSplitNode( topLeftDockID, ImGuiDir_Down, 0.33f, &bottomLeftDockID, &topLeftDockID );
        ImGui::DockBuilderSplitNode( centerDockID, ImGuiDir_Left, 0.66f, &centerDockID, &rightDockID );
        ImGui::DockBuilderSplitNode( rightDockID, ImGuiDir_Down, 0.66f, &bottomRightDockID, &rightDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), rightDockID );
        ImGui::DockBuilderDockWindow( m_debuggerWindowName.c_str(), bottomRightDockID );
        ImGui::DockBuilderDockWindow( m_controlParametersWindowName.c_str(), topLeftDockID );
        ImGui::DockBuilderDockWindow( m_propertyGridWindowName.c_str(), bottomLeftDockID );
        ImGui::DockBuilderDockWindow( m_graphViewWindowName.c_str(), centerDockID );
        ImGui::DockBuilderDockWindow( m_variationEditorWindowName.c_str(), centerDockID );
    }

    void AnimationGraphWorkspace::UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        DebugContext* pDebugContext = nullptr;
        if ( IsPreviewing() && m_pGraphComponent->IsInitialized() )
        {
            pDebugContext = &m_debugContext;
        }

        //-------------------------------------------------------------------------

        if ( m_pControlParameterEditor->UpdateAndDraw( context, pDebugContext, pWindowClass, m_controlParametersWindowName.c_str() ) )
        {
            auto pVirtualParameterToEdit = m_pControlParameterEditor->GetVirtualParameterToEdit();
            if ( pVirtualParameterToEdit != nullptr )
            {
                m_pGraphEditor->NavigateTo( pVirtualParameterToEdit->GetChildGraph() );
            }
        }

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

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        DrawDebuggerWindow( context );
    }

    void AnimationGraphWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        ImGui::SetNextItemWidth( 46 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
        if ( ImGui::BeginCombo( "##RagdollOptions", KRG_ICON_COG, ImGuiComboFlags_HeightLarge ) )
        {
            ImGuiX::TextSeparator( "Root Motion debug" );

            bool const isRootVisualizationOff = m_rootMotionDebugMode == RootMotionRecorder::DebugMode::Off;
            if ( ImGui::RadioButton( "No Visualization", isRootVisualizationOff ) )
            {
                m_rootMotionDebugMode = RootMotionRecorder::DebugMode::Off;
            }

            bool const isRootVisualizationOn = m_rootMotionDebugMode == RootMotionRecorder::DebugMode::DrawRoot;
            if ( ImGui::RadioButton( "Draw Root", isRootVisualizationOn ) )
            {
                m_rootMotionDebugMode = RootMotionRecorder::DebugMode::DrawRoot;
            }

            bool const isRootMotionRecordingEnabled = m_rootMotionDebugMode == RootMotionRecorder::DebugMode::DrawRecordedRootMotion;
            if ( ImGui::RadioButton( "Draw Recorded Root Motion", isRootMotionRecordingEnabled ) )
            {
                m_rootMotionDebugMode = RootMotionRecorder::DebugMode::DrawRecordedRootMotion;
            }

            bool const isAdvancedRootMotionRecordingEnabled = m_rootMotionDebugMode == RootMotionRecorder::DebugMode::DrawRecordedRootMotionAdvanced;
            if ( ImGui::RadioButton( "Draw Advanced Recorded Root Motion", isAdvancedRootMotionRecordingEnabled ) )
            {
                m_rootMotionDebugMode = RootMotionRecorder::DebugMode::DrawRecordedRootMotionAdvanced;
            }

            //-------------------------------------------------------------------------

            ImGuiX::TextSeparator( "Pose Debug" );

            bool const isVisualizationOff = m_taskSystemDebugMode == TaskSystem::DebugMode::Off;
            if ( ImGui::RadioButton( "No Visualization", isVisualizationOff ) )
            {
                m_taskSystemDebugMode = TaskSystem::DebugMode::Off;
            }

            bool const isFinalPoseEnabled = m_taskSystemDebugMode == TaskSystem::DebugMode::FinalPose;
            if ( ImGui::RadioButton( "Final Pose", isFinalPoseEnabled ) )
            {
                m_taskSystemDebugMode = TaskSystem::DebugMode::FinalPose;
            }

            bool const isPoseTreeEnabled = m_taskSystemDebugMode == TaskSystem::DebugMode::PoseTree;
            if ( ImGui::RadioButton( "Pose Tree", isPoseTreeEnabled ) )
            {
                m_taskSystemDebugMode = TaskSystem::DebugMode::PoseTree;
            }

            bool const isDetailedPoseTreeEnabled = m_taskSystemDebugMode == TaskSystem::DebugMode::DetailedPoseTree;
            if ( ImGui::RadioButton( "Detailed Pose Tree", isDetailedPoseTreeEnabled ) )
            {
                m_taskSystemDebugMode = TaskSystem::DebugMode::DetailedPoseTree;
            }

            //-------------------------------------------------------------------------

            ImGuiX::TextSeparator( "Physics" );
            ImGui::BeginDisabled( !IsPreviewing() );
            if ( m_pPhysicsSystem != nullptr && m_pPhysicsSystem->IsConnectedToPVD() )
            {
                if ( ImGui::Button( "Disconnect From PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsSystem->DisconnectFromPVD();
                }
            }
            else
            {
                if ( ImGui::Button( "Connect To PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsSystem->ConnectToPVD();
                }
            }
            ImGui::EndDisabled();

            //-------------------------------------------------------------------------

            ImGui::EndCombo();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        ImGui::SameLine();

        if( BeginViewportToolbarGroup( "Preview", ImVec2( 140, 0 ), ImVec2( 0, 0 ) ) )
        {
            ImVec2 const buttonSize = ImVec2( 116, 0 );
            ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 4, 4 ) );
            if ( IsPreviewing() )
            {
                if ( ImGui::Button( KRG_ICON_STOP"Stop Preview", buttonSize ) )
                {
                    StopPreview();
                }
            }
            else
            {
                if ( ImGui::Button( KRG_ICON_PLAY"Start Preview", buttonSize ) )
                {
                    StartPreview( context );
                }
            }
            ImGui::PopStyleVar();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, 0 );

            ImGui::SetNextItemWidth( 46 );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
            if ( ImGui::BeginCombo( "##PreviewControls", "PreviewControls", ImGuiComboFlags_HeightLarge | ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft ) )
            {
                ImGuiX::TextSeparator( "Preview Settings" );
                ImGui::MenuItem( "Start Paused", nullptr, &m_startPaused );

                ImGuiX::TextSeparator( "Start Transform" );
                ImGuiX::InputTransform( "StartTransform", m_previewStartTransform, 250.0f );

                if ( ImGui::Button( "Reset Start Transform", ImVec2( -1, 0 ) ) )
                {
                    m_previewStartTransform = Transform::Identity;
                }

                ImGui::EndCombo();
            }
            ImGui::PopStyleVar();
        }
        EndViewportToolbarGroup();
    }

    //-------------------------------------------------------------------------

    void AnimationGraphWorkspace::GenerateAnimGraphVariationDescriptors()
    {
        KRG_ASSERT( m_graphFilePath.IsValid() && m_graphFilePath.MatchesExtension( "ag" ) );

        auto const& variations = m_pGraphDefinition->GetVariationHierarchy();
        for ( auto const& variation : variations.GetAllVariations() )
        {
            GraphVariationResourceDescriptor resourceDesc;
            resourceDesc.m_graphPath = GetResourcePath( m_graphFilePath );
            resourceDesc.m_variationID = variation.m_ID;

            InlineString const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, variation.m_ID );
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

    void AnimationGraphWorkspace::OnUndoRedo( UndoStack::Operation operation, IUndoableAction const* pAction )
    {
        TResourceWorkspace<GraphDefinition>::OnUndoRedo( operation, pAction );

        if ( IsPreviewing() )
        {
            StopPreview();
        }

        m_pGraphEditor->OnUndoRedo();
    }

    //-------------------------------------------------------------------------

    void AnimationGraphWorkspace::StartPreview( UpdateContext const& context )
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

        InlineString const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, m_selectedVariationID );
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
                pMeshComponent->SetWorldTransform( m_previewStartTransform );
                m_pPreviewEntity->AddComponent( pMeshComponent );
            }
        }

        // Systems
        //-------------------------------------------------------------------------

        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        // Add the entity
        //-------------------------------------------------------------------------
        
        AddEntityToWorld( m_pPreviewEntity );

        // Set up preview
        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPhysicsSystem == nullptr );
        m_pPhysicsSystem = context.GetSystem<Physics::PhysicsSystem>();
        SetWorldPaused( m_startPaused );
        m_isPreviewing = true;
    }

    void AnimationGraphWorkspace::StopPreview()
    {
        KRG_ASSERT( m_pPhysicsSystem != nullptr );
        if ( m_pPhysicsSystem->IsConnectedToPVD() )
        {
            m_pPhysicsSystem->DisconnectFromPVD();
        }
        m_pPhysicsSystem = nullptr;

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPreviewEntity != nullptr );
        DestroyEntityInWorld( m_pPreviewEntity );
        m_pPreviewEntity = nullptr;
        m_pGraphComponent = nullptr;

        m_debugContext.m_pGraphComponent = nullptr;
        m_debugContext.m_nodeIDtoIndexMap.clear();

        m_isPreviewing = false;
    }

    void AnimationGraphWorkspace::UpdateWorld( EntityWorldUpdateContext const& updateContext )
    {
        bool const isWorldPaused = updateContext.GetDeltaTime() <= 0.0f;
        if ( !IsPreviewing() || !m_pGraphComponent->IsInitialized() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( updateContext.GetUpdateStage() == UpdateStage::FrameEnd )
        {
            if ( !updateContext.IsWorldPaused() )
            {
                Transform const& WT = m_pPreviewEntity->GetWorldTransform();
                Transform const& RMD = m_pGraphComponent->GetRootMotionDelta();
                m_pPreviewEntity->SetWorldTransform( RMD * WT );
            }

            //-------------------------------------------------------------------------

            auto drawingContext = updateContext.GetDrawingContext();
            m_pGraphComponent->SetRootMotionDebugMode( m_rootMotionDebugMode );
            m_pGraphComponent->SetTaskSystemDebugMode( m_taskSystemDebugMode );
            m_pGraphComponent->DrawDebug( drawingContext );
        }
    }

    void AnimationGraphWorkspace::DrawDebuggerWindow( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_debuggerWindowName.c_str() ) )
        {
            if ( IsPreviewing() && m_pGraphComponent->IsInitialized() )
            {
                AnimationDebugView::DrawGraphActiveTasksDebugView( m_pGraphComponent );

                ImGui::NewLine();
                ImGuiX::TextSeparator( "Events" );
                AnimationDebugView::DrawGraphSampledEventsView( m_pGraphComponent );
            }
            else
            {
                ImGui::Text( "Nothing to Debug" );
            }
        }
        ImGui::End();
    }
}