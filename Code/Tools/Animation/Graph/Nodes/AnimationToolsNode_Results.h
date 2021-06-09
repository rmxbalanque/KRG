#pragma once
#include "../AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ResultToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ResultToolsNode );

    public:

        ResultToolsNode() = default;
        ResultToolsNode( NodeValueType type );

        virtual char const* GetTypeName() const override { return "Result"; }
        virtual char const* GetCategory() const override { return "Results"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}