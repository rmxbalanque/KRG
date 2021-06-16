#pragma once
#include "../AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ResultToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ResultToolsNode );

    public:

        ResultToolsNode() = default;
        ResultToolsNode( NodeValueType valueType );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Result"; }
        virtual char const* GetCategory() const override { return "Results"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        REGISTER NodeValueType m_valueType = NodeValueType::Pose;
    };
}