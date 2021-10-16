#include "AnimationToolsGraph.h"
#include "AnimationToolsGraph_Compilation.h"
#include "Nodes/AnimationToolsNode_Results.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    AnimationToolsGraph::~AnimationToolsGraph()
    {
        ResetInternalState();
    }

    void AnimationToolsGraph::ResetInternalState()
    {
        m_variationHierarchy.Reset();

        //-------------------------------------------------------------------------

        for ( auto pParameter : m_controlParameters )
        {
            pParameter->Shutdown();
            KRG::Delete( pParameter );
        }
        m_controlParameters.clear();

        for ( auto pParameter : m_virtualParameters )
        {
            pParameter->Shutdown();
            KRG::Delete( pParameter );
        }
        m_virtualParameters.clear();

        //-------------------------------------------------------------------------

        if ( m_pRootGraph != nullptr )
        {
            m_pRootGraph->Shutdown();
            KRG::Delete( m_pRootGraph );
        }
    }

    void AnimationToolsGraph::CreateNew()
    {
        ResetInternalState();

        // Create root blend tree
        m_pRootGraph = KRG::New<FlowGraph>( GraphType::BlendTree );
        m_pRootGraph->CreateNode<Tools_ResultNode>( ValueType::Pose );
    }

    bool AnimationToolsGraph::Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphDescriptorObjectValue )
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

        m_pRootGraph = Cast<FlowGraph>( GraphEditor::BaseGraph::CreateGraphFromSerializedData( typeRegistry, rootGraphValueIter->value, nullptr ) );

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

        // Control Parameters
        //-------------------------------------------------------------------------

        auto controlParametersValueIter = graphObjectValue.FindMember( "ControlParameters" );
        if ( controlParametersValueIter == graphObjectValue.MemberEnd() )
        {
            return false;
        }

        for ( auto& nodeObjectValue : controlParametersValueIter->value.GetArray() )
        {
            auto pNewParameter = GraphEditor::BaseNode::CreateNodeFromSerializedData( typeRegistry, nodeObjectValue, nullptr );
            m_controlParameters.emplace_back( Cast<Tools_ControlParameterNode>( pNewParameter ) );
        }

        // Fix reference node ptrs
        auto const controlParameterReferences = FindAllNodesOfType<Tools_ControlParameterReferenceNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Exact );
        for ( auto const& pParameter : controlParameterReferences )
        {
            auto pFoundParameter = FindControlParameter( pParameter->GetReferencedParameterID() );
            if ( pFoundParameter != nullptr )
            {
                pParameter->m_pParameter = pFoundParameter;
            }
            else // Invalid graph data encountered
            {
                return false;
            }
        }

        // Virtual Parameters
        //-------------------------------------------------------------------------

        auto virtualParametersValueIter = graphObjectValue.FindMember( "VirtualParameters" );
        if( virtualParametersValueIter == graphObjectValue.MemberEnd() )
        {
            return false;
        }

        for ( auto& nodeObjectValue : virtualParametersValueIter->value.GetArray() )
        {
            auto pNewParameter = GraphEditor::BaseNode::CreateNodeFromSerializedData( typeRegistry, nodeObjectValue, nullptr );
            m_virtualParameters.emplace_back( Cast<Tools_VirtualParameterNode>( pNewParameter ) );
        }

        // Fix reference node ptrs
        auto const virtualParameterReferences = FindAllNodesOfType<Tools_VirtualParameterReferenceNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Exact );
        for ( auto const& pParameter : virtualParameterReferences )
        {
            auto pFoundParameter = FindVirtualParameter( pParameter->GetReferencedParameterID() );
            if ( pFoundParameter != nullptr )
            {
                pParameter->m_pParameter = pFoundParameter;
            }
            else // Invalid graph data encountered
            {
                return false;
            }
        }

        return true;
    }

    void AnimationToolsGraph::Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        // Write descriptor header
        writer.StartObject();
        writer.Key( "TypeID" );
        writer.String( "KRG::Animation::AnimationGraphResourceDescriptor" );

        writer.Key( "GraphDefinition" );

        //-------------------------------------------------------------------------

        {
            writer.StartObject();

            writer.Key( "RootGraph" );
            m_pRootGraph->Serialize( typeRegistry, writer );

            //-------------------------------------------------------------------------

            writer.Key( "Variations" );
            m_variationHierarchy.Serialize( typeRegistry, writer );

            //-------------------------------------------------------------------------

            writer.Key( "ControlParameters" );
            writer.StartArray();

            for ( auto pNode : m_controlParameters )
            {
                pNode->Serialize( typeRegistry, writer );
            }

            writer.EndArray();

            //-------------------------------------------------------------------------

            writer.Key( "VirtualParameters" );
            writer.StartArray();

            for ( auto pNode : m_virtualParameters )
            {
                pNode->Serialize( typeRegistry, writer );
            }

            writer.EndArray();

            //-------------------------------------------------------------------------

            writer.EndObject();
        }

        //-------------------------------------------------------------------------

        writer.EndObject();
    }

    //-------------------------------------------------------------------------

    void AnimationToolsGraph::CreateControlParameter( ValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        auto pParameter = KRG::New<Tools_ControlParameterNode>( parameterName, type );
        pParameter->Initialize( nullptr );
        m_controlParameters.emplace_back( pParameter );
    }

    void AnimationToolsGraph::CreateVirtualParameter( ValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        auto pParameter = KRG::New<Tools_VirtualParameterNode>( parameterName, type );
        pParameter->Initialize( nullptr );
        m_virtualParameters.emplace_back( pParameter );
    }

    void AnimationToolsGraph::RenameControlParameter( UUID parameterID, String newName )
    {
        auto pParameter = FindControlParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );
        EnsureUniqueParameterName( newName );
        pParameter->m_name = newName;
    }

    void AnimationToolsGraph::RenameVirtualParameter( UUID parameterID, String newName )
    {
        auto pParameter = FindVirtualParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );
        EnsureUniqueParameterName( newName );
        pParameter->m_name = newName;
    }

    void AnimationToolsGraph::DestroyControlParameter( UUID parameterID )
    {
        // Find and remove all reference nodes
        auto const controlParameterReferences = FindAllNodesOfType<Tools_ControlParameterReferenceNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Exact );

        for ( auto const& pFoundParameterNode : controlParameterReferences )
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
                pParameter->Shutdown();
                KRG::Delete( pParameter );
                m_controlParameters.erase( iter );
                break;
            }
        }
    }

    void AnimationToolsGraph::DestroyVirtualParameter( UUID parameterID )
    {
        KRG_ASSERT( FindVirtualParameter( parameterID ) != nullptr );

        // Find and remove all reference nodes
        auto const virtualParameterReferences = FindAllNodesOfType<Tools_VirtualParameterReferenceNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Exact );

        for ( auto const& pFoundParameterNode : virtualParameterReferences )
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
                pParameter->Shutdown();
                KRG::Delete( pParameter );
                m_virtualParameters.erase( iter );
                break;
            }
        }
    }

    void AnimationToolsGraph::EnsureUniqueParameterName( String& parameterName ) const
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

    Tools_ControlParameterNode* AnimationToolsGraph::FindControlParameter( UUID parameterID ) const
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

    Tools_VirtualParameterNode* AnimationToolsGraph::FindVirtualParameter( UUID parameterID ) const
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

    bool AnimationToolsGraph::Compile( ToolsGraphCompilationContext& context ) const
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

        auto const resultNodes = m_pRootGraph->FindAllNodesOfType<Tools_ResultNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        context.m_rootNodeIdx = resultNodes[0]->Compile( context );
        context.m_persistentNodeIndices.emplace_back( context.m_rootNodeIdx );

        return context.m_rootNodeIdx != InvalidIndex;
    }
}