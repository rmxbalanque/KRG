#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class SpeedScaleEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SpeedScaleEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Speed Scale"; }
        virtual char const* GetCategory() const override { return "Utility"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FloatRange              m_scaleLimits = FloatRange( 0, 0 );
        KRG_EXPOSE float                   m_blendTime = 0.2f;
    };

    //-------------------------------------------------------------------------

    class VelocityBasedSpeedScaleEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( VelocityBasedSpeedScaleEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Velocity Based Speed Scale"; }
        virtual char const* GetCategory() const override { return "Utility"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE float                   m_blendTime = 0.2f;
    };
}