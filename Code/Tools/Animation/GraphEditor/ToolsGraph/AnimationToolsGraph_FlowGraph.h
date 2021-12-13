#pragma once
#include "AnimationToolsGraph_FlowNode.h"
#include "Tools/Core/GraphEditor/Flow/FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class FlowGraph : public GraphEditor::FlowGraph
    {
        friend class AnimationGraphToolsDefinition;
        KRG_REGISTER_TYPE( FlowGraph );

    public:

        FlowGraph( GraphType type = GraphType::BlendTree );

        inline GraphType GetType() const { return m_type; }

        // Node factory methods
        //-------------------------------------------------------------------------

        template<typename T, typename ... ConstructorParams>
        T* CreateNode( ConstructorParams&&... params )
        {
            GraphEditor::ScopedGraphModification sgm( this );

            static_assert( std::is_base_of<Tools_GraphNode, T>::value );
            auto pNode = KRG::New<T>( std::forward<ConstructorParams>( params )... );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            pNode->Initialize( this );
            AddNode( pNode );
            return pNode;
        }

        Tools_GraphNode* CreateNode( TypeSystem::TypeInfo const* pTypeInfo )
        {
            GraphEditor::ScopedGraphModification sgm( this );

            KRG_ASSERT( pTypeInfo->IsDerivedFrom( Tools_GraphNode::GetStaticTypeID() ) );
            auto pNode = Cast<Tools_GraphNode>( pTypeInfo->m_pTypeHelper->CreateType() );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            pNode->Initialize( this );
            AddNode( pNode );
            return pNode;
        }

    private:

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

    private:

        KRG_REGISTER GraphType       m_type;
    };
}