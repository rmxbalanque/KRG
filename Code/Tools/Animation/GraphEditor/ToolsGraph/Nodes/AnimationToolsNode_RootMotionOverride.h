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

        // Set to negative to disable the velocity limiter
        KRG_EXPOSE float                m_maxLinearVelocity = -1.0f;

        // Set to negative to disable the velocity limiter
        KRG_EXPOSE float                m_maxAngularVelocity = -1.0f;

        KRG_EXPOSE bool                 m_overrideHeadingX = true;
        KRG_EXPOSE bool                 m_overrideHeadingY = true;
        KRG_EXPOSE bool                 m_overrideHeadingZ = true;
        KRG_EXPOSE bool                 m_overrideFacingX = true;
        KRG_EXPOSE bool                 m_overrideFacingY = true;
        KRG_EXPOSE bool                 m_overrideFacingZ = true;
    };
}