#include "Animation_EditorGraph_Definition.h"
#include "Animation_EditorGraph_Compilation.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace KRG::Animation::GraphNodes;

    //-------------------------------------------------------------------------

    EditorGraphDefinition::~EditorGraphDefinition()
    {
        ResetInternalState();
    }

    void EditorGraphDefinition::ResetInternalState()
    {
        m_variationHierarchy.Reset();
        m_controlParameters.clear();
        m_virtualParameters.clear();

        //-------------------------------------------------------------------------

        if ( m_pRootGraph != nullptr )
        {
            m_pRootGraph->Shutdown();
            KRG::Delete( m_pRootGraph );
        }
    }

    void EditorGraphDefinition::CreateNew()
    {
        ResetInternalState();

        // Create root blend tree
        m_pRootGraph = KRG::New<FlowGraph>( GraphType::BlendTree );
        m_pRootGraph->CreateNode<ResultEditorNode>( GraphValueType::Pose );
    }

    bool EditorGraphDefinition::LoadFromJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphDescriptorObjectValue )
    {
        KRG_ASSERT( graphDescriptorObjectValue.IsObject() );

        ResetInternalState();

        // Get Graph Data object
        //-------------------------------------------------------------------------

        auto graphDefinitionValueIter = graphDescriptorObjectValue.FindMember( "GraphDefinition" );
        if ( graphDefinitionValueIter == graphDescriptorObjectValue.MemberEnd() || !graphDefinitionValueIter->value.IsObject() )
        {
            return false;
        }

        RapidJsonValue const& graphObjectValue = graphDefinitionValueIter->value;

        // Root Graph
        //-------------------------------------------------------------------------

        auto rootGraphValueIter = graphObjectValue.FindMember( "RootGraph" );
        if ( rootGraphValueIter == graphObjectValue.MemberEnd() )
        {
            return false;
        }

        m_pRootGraph = Cast<FlowGraph>( VisualGraph::BaseGraph::CreateGraphFromSerializedData( typeRegistry, rootGraphValueIter->value, nullptr ) );

        // Variations
        //-------------------------------------------------------------------------

        auto variationsValueIter = graphObjectValue.FindMember( "Variations" );
        if ( variationsValueIter == graphObjectValue.MemberEnd() || !variationsValueIter->value.IsArray() )
        {
            return false;
        }

        if ( !m_variationHierarchy.Serialize( typeRegistry, variationsValueIter->value ) )
        {
            return false;
        }

        // Parameters
        //-------------------------------------------------------------------------

        m_controlParameters = m_pRootGraph->FindAllNodesOfType<ControlParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );
        m_virtualParameters = m_pRootGraph->FindAllNodesOfType<VirtualParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );

        ParameterReferenceEditorNode::RefreshParameterReferences( m_pRootGraph );

        return true;
    }

    void EditorGraphDefinition::SaveToJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.StartObject();
        writer.Key( "TypeID" );
        writer.String( GraphResourceDescriptor::GetStaticTypeID().c_str() );

        writer.Key( "GraphDefinition" );
        writer.StartObject();
        {
            writer.Key( "RootGraph" );
            m_pRootGraph->Serialize( typeRegistry, writer );

            writer.Key( "Variations" );
            m_variationHierarchy.Serialize( typeRegistry, writer );

            writer.EndObject();
        }
        writer.EndObject();
    }

    //-------------------------------------------------------------------------

    void EditorGraphDefinition::CreateControlParameter( GraphValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );
        auto pParameter = m_pRootGraph->CreateNode<ControlParameterEditorNode>( parameterName, type );
        m_controlParameters.emplace_back( pParameter );
    }

    void EditorGraphDefinition::CreateVirtualParameter( GraphValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );
        auto pParameter = m_pRootGraph->CreateNode<VirtualParameterEditorNode>( parameterName, type );
        m_virtualParameters.emplace_back( pParameter );
    }

    void EditorGraphDefinition::RenameControlParameter( UUID parameterID, String const& newName, String const& category )
    {
        auto pParameter = FindControlParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );

        String uniqueName = newName;
        EnsureUniqueParameterName( uniqueName );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );
        pParameter->m_name = uniqueName;
        pParameter->m_parameterCategory = category;
    }

    void EditorGraphDefinition::RenameVirtualParameter( UUID parameterID, String const& newName, String const& category )
    {
        auto pParameter = FindVirtualParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );
        
        String uniqueName = newName;
        EnsureUniqueParameterName( uniqueName );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );
        pParameter->m_name = uniqueName;
        pParameter->m_parameterCategory = category;
    }

    void EditorGraphDefinition::DestroyControlParameter( UUID parameterID )
    {
        KRG_ASSERT( FindControlParameter( parameterID ) != nullptr );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );

        // Find and remove all reference nodes
        auto const parameterReferences = FindAllNodesOfType<ParameterReferenceEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Exact );
        for ( auto const& pFoundParameterNode : parameterReferences )
        {
            if ( pFoundParameterNode->GetReferencedParameterID() == parameterID )
            {
                pFoundParameterNode->Destroy();
            }
        }

        // Delete parameter
        for ( auto iter = m_controlParameters.begin(); iter != m_controlParameters.end(); ++iter )
        {
            auto pParameter = ( *iter );
            if ( pParameter->m_ID == parameterID )
            {
                pParameter->Destroy();
                m_controlParameters.erase( iter );
                break;
            }
        }
    }

    void EditorGraphDefinition::DestroyVirtualParameter( UUID parameterID )
    {
        KRG_ASSERT( FindVirtualParameter( parameterID ) != nullptr );

        VisualGraph::ScopedGraphModification gm( m_pRootGraph );

        // Find and remove all reference nodes
        auto const parameterReferences = FindAllNodesOfType<ParameterReferenceEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Exact );
        for ( auto const& pFoundParameterNode : parameterReferences )
        {
            if ( pFoundParameterNode->GetReferencedParameterID() == parameterID )
            {
                pFoundParameterNode->Destroy();
            }
        }

        // Delete parameter
        for ( auto iter = m_virtualParameters.begin(); iter != m_virtualParameters.end(); ++iter )
        {
            auto pParameter = ( *iter );
            if ( pParameter->m_ID == parameterID )
            {
                pParameter->Destroy();
                m_virtualParameters.erase( iter );
                break;
            }
        }
    }

    void EditorGraphDefinition::EnsureUniqueParameterName( String& parameterName ) const
    {
        String tempString = parameterName;
        bool isNameUnique = false;
        int32 suffix = 0;

        while ( !isNameUnique )
        {
            isNameUnique = true;

            // Check control parameters
            for ( auto pParameter : m_controlParameters )
            {
                if ( pParameter->m_name == tempString )
                {
                    isNameUnique = false;
                    break;
                }
            }

            // Check virtual parameters
            if ( isNameUnique )
            {
                for ( auto pParameter : m_virtualParameters )
                {
                    if ( pParameter->m_name == tempString )
                    {
                        isNameUnique = false;
                        break;
                    }
                }
            }

            if ( !isNameUnique )
            {
                tempString.sprintf( "%s_%d", parameterName.c_str(), suffix );
                suffix++;
            }
        }

        //-------------------------------------------------------------------------

        parameterName = tempString;
    }

    ControlParameterEditorNode* EditorGraphDefinition::FindControlParameter( UUID parameterID ) const
    {
        for ( auto pParameter : m_controlParameters )
        {
            if ( pParameter->m_ID == parameterID )
            {
                return pParameter;
            }
        }
        return nullptr;
    }

    VirtualParameterEditorNode* EditorGraphDefinition::FindVirtualParameter( UUID parameterID ) const
    {
        for ( auto pParameter : m_virtualParameters )
        {
            if ( pParameter->m_ID == parameterID )
            {
                return pParameter;
            }
        }
        return nullptr;
    }

    bool EditorGraphDefinition::Compile( EditorGraphCompilationContext& context ) const
    {
        KRG_ASSERT( IsValid() );

        // Always compile control parameters first
        //-------------------------------------------------------------------------

        for ( auto pParameter : m_controlParameters )
        {
            if ( pParameter->Compile( context ) == InvalidIndex )
            {
                return false;
            }
        }

        context.m_numControlParameters = (uint32) m_controlParameters.size();

        for ( auto i = 0; i < context.m_numControlParameters; i++ )
        {
            context.m_controlParameterIDs.emplace_back( m_controlParameters[i]->GetParameterID() );
        }

        // Then Virtual parameters
        //-------------------------------------------------------------------------

        for ( auto pParameter : m_virtualParameters )
        {
            if ( pParameter->Compile( context ) == InvalidIndex )
            {
                return false;
            }
        }

        // Finally compile the actual graph
        //-------------------------------------------------------------------------

        auto const resultNodes = m_pRootGraph->FindAllNodesOfType<ResultEditorNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        context.m_rootNodeIdx = resultNodes[0]->Compile( context );
        context.m_persistentNodeIndices.emplace_back( context.m_rootNodeIdx );

        return context.m_rootNodeIdx != InvalidIndex;
    }
}