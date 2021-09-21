#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_LayerSettingsNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_LayerSettingsNode );

        friend class Tools_LayerNode;

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Layer"; }
        virtual char const* GetCategory() const override { return "Layers"; }
        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::PaleGoldenRod ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;

    private:

        EXPOSE bool                             m_isSynchronized = false;
        EXPOSE bool                             m_ignoreEvents = false;
        EXPOSE TBitFlags<PoseBlendOptions>      m_blendOptions;
    };

    //-------------------------------------------------------------------------

    class Tools_LayerNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_LayerNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Layer Blend"; }
        virtual char const* GetCategory() const override { return "Layers"; }
        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::PaleGoldenRod ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Unknown; }
        virtual void OnDynamicPinDestruction( UUID pinID ) override;

        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE bool                             m_onlySampleBaseRootMotion = true;
    };
}