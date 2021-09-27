#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_SpeedScaleNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_SpeedScaleNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Speed Scale"; }
        virtual char const* GetCategory() const override { return "Anim"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FloatRange              m_scaleLimits = FloatRange( 0, 0 );
        KRG_EXPOSE float                   m_blendTime = 0.2f;
    };

    //-------------------------------------------------------------------------

    class Tools_VelocityBasedSpeedScaleNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_VelocityBasedSpeedScaleNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Velocity Based Speed Scale"; }
        virtual char const* GetCategory() const override { return "Anim"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE float                   m_blendTime = 0.2f;
    };
}