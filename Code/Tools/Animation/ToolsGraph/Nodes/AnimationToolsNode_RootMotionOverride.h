#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_RootMotionOverrideNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_RootMotionOverrideNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Root Motion Override"; }
        virtual char const* GetCategory() const override { return "Anim"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE float                            m_maxLinearVelocity = 0.0f;
        EXPOSE float                            m_maxAngularVelocity = 0.0f;
        EXPOSE bool                             m_overrideHeadingX = true;
        EXPOSE bool                             m_overrideHeadingY = true;
        EXPOSE bool                             m_overrideHeadingZ = true;
        EXPOSE bool                             m_overrideFacingX = true;
        EXPOSE bool                             m_overrideFacingY = true;
        EXPOSE bool                             m_overrideFacingZ = true;
    };
}