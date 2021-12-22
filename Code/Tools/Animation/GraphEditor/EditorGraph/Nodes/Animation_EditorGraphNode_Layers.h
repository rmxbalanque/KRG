#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class LayerSettingsEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( LayerSettingsEditorNode );

        friend class LayerEditorNode;

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Layer"; }
        virtual char const* GetCategory() const override { return "Blends/Layers"; }
        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::PaleGoldenRod ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;

    private:

        KRG_EXPOSE bool                             m_isSynchronized = false;
        KRG_EXPOSE bool                             m_ignoreEvents = false;
        KRG_EXPOSE TBitFlags<PoseBlendOptions>      m_blendOptions;
    };

    //-------------------------------------------------------------------------

    class LayerEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( LayerEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Layer Blend"; }
        virtual char const* GetCategory() const override { return "Blends/Layers"; }
        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::PaleGoldenRod ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Unknown; }
        virtual void OnDynamicPinDestruction( UUID pinID ) override;

        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE bool                             m_onlySampleBaseRootMotion = true;
    };
}