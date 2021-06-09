#pragma once
#include "AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraph final : public GraphEditor::FlowGraph
    {
        friend class ToolsAnimationGraph;

    public:

        ToolsGraph( GraphType type = GraphType::BlendTree );

        inline GraphType GetType() const { return m_type; }

        //-------------------------------------------------------------------------

        // Node factory method
        template<typename T, typename ... ConstructorParams>
        T* CreateNode( ConstructorParams&&... params )
        {
            static_assert( std::is_base_of<ToolsNode, T>::value );
            auto pNode = KRG::New<T>( std::forward<ConstructorParams>( params )... );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            AddNode( pNode );
            return pNode;
        }

        ToolsNode* CreateNode( TypeSystem::TypeInfo const* pTypeInfo )
        {
            KRG_ASSERT( pTypeInfo->IsDerivedFrom( ToolsNode::GetStaticTypeID() ) );
            auto pNode = SafeCast<ToolsNode>( pTypeInfo->m_pTypeHelper->CreateType() );
            KRG_ASSERT( pNode->GetAllowedParentGraphTypes().IsFlagSet( m_type ) );
            AddNode( pNode );
            return pNode;
        }

    private:

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

    private:

        GraphType       m_type;
    };
}