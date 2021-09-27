#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Blends.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ParameterizedBlendNode : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ParameterizedBlendNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetCategory() const override { return "Blends"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override final { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "Input"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Pose; }

    protected:

        bool CompileParameterAndSourceNodes( ToolsGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const;

    protected:

        KRG_EXPOSE bool                 m_isSynchronized = false;
    };

    //-------------------------------------------------------------------------

    class Tools_RangedBlendNode final : public Tools_ParameterizedBlendNode
    {
        KRG_REGISTER_TYPE( Tools_RangedBlendNode );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Ranged Blend"; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual bool DrawPinControls( GraphEditor::Flow::Pin const& pin ) override;
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;

    private:

        KRG_REGISTER TVector<float>     m_parameterValues;
    };

    //-------------------------------------------------------------------------

    class Tools_VelocityBlendNode final : public Tools_ParameterizedBlendNode
    {
        KRG_REGISTER_TYPE( Tools_VelocityBlendNode );

        virtual char const* GetTypeName() const override { return "Velocity Blend"; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}