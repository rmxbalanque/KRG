#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Blends.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ParameterizedBlendEditorNode : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ParameterizedBlendEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetCategory() const override { return "Blends"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override final { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "Input"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Pose; }

    protected:

        bool CompileParameterAndSourceNodes( EditorGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const;

    protected:

        KRG_EXPOSE bool                 m_isSynchronized = false;
    };

    //-------------------------------------------------------------------------

    class RangedBlendEditorNode final : public ParameterizedBlendEditorNode
    {
        KRG_REGISTER_TYPE( RangedBlendEditorNode );

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Ranged Blend"; }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual bool DrawPinControls( VisualGraph::Flow::Pin const& pin ) override;
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;

    private:

        KRG_REGISTER TVector<float>     m_parameterValues;
    };

    //-------------------------------------------------------------------------

    class VelocityBlendEditorNode final : public ParameterizedBlendEditorNode
    {
        KRG_REGISTER_TYPE( VelocityBlendEditorNode );

        virtual char const* GetTypeName() const override { return "Velocity Blend"; }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };
}