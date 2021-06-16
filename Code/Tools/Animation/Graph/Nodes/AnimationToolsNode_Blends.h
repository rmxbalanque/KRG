#pragma once
#include "../AnimationGraphTools_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Blends.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ParameterizedBlendToolsNode : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ParameterizedBlendToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetCategory() const override { return "Blends"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override final { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "Input"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) NodeValueType::Pose; }

    protected:

        bool CompileParameterAndSourceNodes( ToolsGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const;

    protected:

        EXPOSE bool                 m_isSynchronized = false;
    };

    //-------------------------------------------------------------------------

    class RangedBlendToolsNode final : public ParameterizedBlendToolsNode
    {
        KRG_REGISTER_TYPE( RangedBlendToolsNode );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Ranged Blend"; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual bool DrawPinControls( GraphEditor::Flow::Pin const& pin ) override;
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;

    private:

        REGISTER TVector<float>     m_parameterValues;
    };

    //-------------------------------------------------------------------------

    class VelocityBlendToolsNode final : public ParameterizedBlendToolsNode
    {
        KRG_REGISTER_TYPE( VelocityBlendToolsNode );

        virtual char const* GetTypeName() const override { return "Velocity Blend"; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}