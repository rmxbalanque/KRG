#include "AnimationGraphEditor_Model.h"
#include "ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Engine/Animation/Systems/EntitySystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimatedMeshes.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    static InlineString<255> GenerateFilePathForVariation( FileSystem::Path const& graphPath, StringID variationID )
    {
        FileSystem::Path const parentDirectory = graphPath.GetParentDirectory();
        String const filenameNoExtension = graphPath.GetFileNameWithoutExtension();

        InlineString<255> variationPathStr;
        variationPathStr.sprintf( "%s%s_%s.agv", parentDirectory.c_str(), filenameNoExtension.c_str(), variationID.c_str() );
        return variationPathStr;
    }

    //-------------------------------------------------------------------------

    GraphEditorModel::GraphEditorModel( EditorContext const& editorContext, ResourceID const& graphResourceID )
        : m_editorContext( editorContext )
    {
        // Create DB of all node types
        //-------------------------------------------------------------------------

        m_registeredNodeTypes = m_editorContext.m_pTypeRegistry->GetAllDerivedTypes( Tools_GraphNode::GetStaticTypeID(), false, false );

        for ( auto pNodeType : m_registeredNodeTypes )
        {
            auto pDefaultNode = Cast<Tools_GraphNode const>( pNodeType->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( pDefaultNode->IsUserCreatable() )
            {
                m_categorizedNodeTypes.AddItem( pDefaultNode->GetCategory(), pDefaultNode->GetTypeName(), pNodeType );
            }
        }

        // Load graph from descriptor
        //-------------------------------------------------------------------------

        m_graphFilePath = graphResourceID.GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
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
            m_pGraph = KRG::New<AnimationToolsGraph>();
            if ( m_pGraph->Load( *m_editorContext.m_pTypeRegistry, reader.GetDocument() ) )
            {
                m_pViewedGraph = m_pGraph->GetRootGraph();
            }
            else
            {
                graphLoadFailed = true;
            }
            
            // Load failed, so clean up and create a new graph
            if( graphLoadFailed )
            {
                KRG::Delete( m_pGraph );
                m_pGraph = KRG::New<AnimationToolsGraph>();
                m_pGraph->CreateNew();
                m_pViewedGraph = m_pGraph->GetRootGraph();
            }
        }
    }

    GraphEditorModel::~GraphEditorModel()
    {
        KRG_ASSERT( !m_isPreviewing );
        KRG::Delete( m_pGraph );
    }

    //-------------------------------------------------------------------------

    bool GraphEditorModel::SaveGraph()
    {
        KRG_ASSERT( m_graphFilePath.IsValid() );
        JsonWriter writer;
        m_pGraph->Save( *m_editorContext.m_pTypeRegistry, *writer.GetWriter() );
        if ( writer.WriteToFile( m_graphFilePath ) )
        {
            GenerateAnimGraphVariationDescriptors();
            return true;
        }

        return false;
    }

    void GraphEditorModel::GenerateAnimGraphVariationDescriptors()
    {
        KRG_ASSERT( m_graphFilePath.IsValid() && m_graphFilePath.MatchesExtension( "ag" ) );

        auto const& variations = m_pGraph->GetVariationHierarchy();
        for ( auto const& variation : variations.GetAllVariations() )
        {
            AnimationGraphVariationResourceDescriptor resourceDesc;
            resourceDesc.m_graphPath = ResourcePath::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, m_graphFilePath );
            resourceDesc.m_variationID = variation.m_ID;

            InlineString<255> const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, variation.m_ID );
            FileSystem::Path const variationPath( variationPathStr.c_str() );

            WriteResourceDescriptorToFile( *m_editorContext.m_pTypeRegistry, variationPath, &resourceDesc );
        }
    }

    void GraphEditorModel::NavigateTo( GraphEditor::BaseNode const* pNode )
    {
        KRG_ASSERT( pNode != nullptr );
        m_pViewedGraph = const_cast<GraphEditor::BaseNode*>( pNode )->GetParentGraph();
        KRG_ASSERT( m_pViewedGraph != nullptr );
    }

    void GraphEditorModel::NavigateTo( GraphEditor::BaseGraph const* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );
        m_pViewedGraph = const_cast<GraphEditor::BaseGraph*>( pGraph );
    }

    void GraphEditorModel::StartPreview( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( pPreviewWorld != nullptr );

        // Try to compile the graph
        //-------------------------------------------------------------------------

        ToolsGraphCompilationContext compilationContext;
        if ( m_pGraph->Compile( compilationContext ) )
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

        SaveGraph();

        //-------------------------------------------------------------------------

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );

        // Graph Component
        //-------------------------------------------------------------------------

        InlineString<255> const variationPathStr = GenerateFilePathForVariation( m_graphFilePath, m_selectedVariationID );
        ResourceID const graphVariationResourceID( ResourcePath::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, variationPathStr.c_str() ) );

        m_pGraphComponent = KRG::New<AnimationGraphComponent>( StringID( "Animation Component" ) );
        m_pGraphComponent->SetGraphVariation( graphVariationResourceID );
        m_pPreviewEntity->AddComponent( m_pGraphComponent );

        m_debugContext.m_pGraphComponent = m_pGraphComponent;

        // Preview Mesh Component
        //-------------------------------------------------------------------------

        auto pVariation = m_pGraph->GetVariation( m_selectedVariationID );
        KRG_ASSERT( pVariation != nullptr );
        if ( pVariation->m_pSkeleton.IsValid() )
        {
            // Load resource descriptor for skeleton to get the preview mesh
            FileSystem::Path const resourceDescPath = pVariation->m_pSkeleton.GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
            SkeletonResourceDescriptor resourceDesc;
            if ( TryReadResourceDescriptorFromFile( *m_editorContext.m_pTypeRegistry, resourceDescPath, resourceDesc ) )
            {
                // Create a preview mesh component
                auto pMeshComponent = KRG::New<AnimatedMeshComponent>( StringID( "Mesh Component" ) );
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
        // We dont own the entity as soon as we add it to the map

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->AddEntity( m_pPreviewEntity );
        m_isPreviewing = true;
    }

    void GraphEditorModel::StopPreview( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( pPreviewWorld != nullptr );
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );

        m_pPreviewEntity = nullptr;
        m_pGraphComponent = nullptr;

        m_debugContext.m_pGraphComponent = nullptr;
        m_debugContext.m_nodeIDtoIndexMap.clear();

        m_isPreviewing = false;
    }
}