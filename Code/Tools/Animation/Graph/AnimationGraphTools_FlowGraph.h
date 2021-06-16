#pragma once
#include "AnimationGraphTools_FlowNode.h"
#include "Tools/Core/GraphEditor/Flow/FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class FlowToolGraph : public GraphEditor::FlowGraph
    {
        friend class ToolsAnimationGraph;
        KRG_REGISTER_TYPE( FlowToolGraph );

    public:

        FlowToolGraph( GraphType type = GraphType::BlendTree );

        inline GraphType GetType() const { return m_type; }

        // Node factory methods
        //-------------------------------------------------------------------------

        template<typename T, typename ... ConstructorParams>
        T* CreateNode( ConstructorParams&&... params )
        {
            static_assert( std::is_base_of<FlowToolsNode, T>::value );
            auto pNode = KRG::New<T>( std::forward<ConstructorParams>( params )... );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            pNode->Initialize( this );
            AddNode( pNode );
            return pNode;
        }

        FlowToolsNode* CreateNode( TypeSystem::TypeInfo const* pTypeInfo )
        {
            KRG_ASSERT( pTypeInfo->IsDerivedFrom( FlowToolsNode::GetStaticTypeID() ) );
            auto pNode = Cast<FlowToolsNode>( pTypeInfo->m_pTypeHelper->CreateType() );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            pNode->Initialize( this );
            AddNode( pNode );
            return pNode;
        }

    private:

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

    private:

        REGISTER GraphType       m_type;
    };
}