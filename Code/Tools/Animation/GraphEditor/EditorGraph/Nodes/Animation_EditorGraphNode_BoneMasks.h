#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class BoneMaskEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( BoneMaskEditorNode );

        // TEMP: until we have a better editor
        struct ToolsBoneWeight : public IRegisteredType
        {
            KRG_REGISTER_TYPE( ToolsBoneWeight )

            StringID        m_boneID;
            float           m_weight;
        };

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Bone Mask"; }
        virtual char const* GetCategory() const override { return "Values/Bone Mask"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE float                                m_rootMotionWeight = 1.0f;
        KRG_EXPOSE TVector<ToolsBoneWeight>             m_weights;
    };

    //-------------------------------------------------------------------------

    class BoneMaskBlendEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( BoneMaskBlendEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Bone Mask Blend"; }
        virtual char const* GetCategory() const override { return "Values/Bone Mask"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class BoneMaskSelectorEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( BoneMaskSelectorEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Bone Mask Selector"; }
        virtual char const* GetCategory() const override { return "Values/Bone Mask"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree, GraphType::TransitionTree ); }

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "Mask"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::BoneMask; }

        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE bool                                   m_switchDynamically = false;
        KRG_EXPOSE TVector<StringID>                      m_parameterValues;
        KRG_EXPOSE Seconds                                m_blendTime = 0.1f;
    };
}