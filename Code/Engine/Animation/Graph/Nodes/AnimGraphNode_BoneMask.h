#pragma once
#include "Engine/Animation/Graph/AnimGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class BoneMaskNode : public ValueNodeBoneMask
    {
    public:

        struct Settings : public ValueNodeBoneMask::Settings
        {
            float                               m_rootMotionWeight = 1.0f;
            TVector<BoneWeight>                 m_weights;
        };

    public:

        inline BoneMask const& GetBoneMask() const { return m_boneMask; }

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoneMask                                m_boneMask;
    };

    //-------------------------------------------------------------------------

    class BoneMaskBlendNode : public ValueNodeBoneMask
    {

    public:

        struct Settings : public ValueNodeBoneMask::Settings
        {
            NodeIndex                           m_sourceMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_targetMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_blendWeightValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;

    private:

        ValueNodeBoneMask*                      m_pSourceBoneMask = nullptr;
        ValueNodeBoneMask*                      m_pTargetBoneMask = nullptr;
        ValueNodeFloat*                         m_pBlendWeightValueNode = nullptr;
        BoneMask                                m_blendedBoneMask;
    };

    //-------------------------------------------------------------------------

    class BoneMaskSelectorNode : public ValueNodeBoneMask
    {
    public:

        struct Settings : public ValueNodeBoneMask::Settings
        {
            NodeIndex                                       m_defaultMaskNodeIdx = InvalidIndex;
            NodeIndex                                       m_parameterValueNodeIdx = InvalidIndex;
            bool                                            m_switchDynamically = false;
            TInlineVector<NodeIndex, 7>                     m_maskNodeIndices;
            TInlineVector<StringID, 7>                      m_parameterValues;
            Seconds                                         m_blendTime = 0.1f;
        };

        #if KRG_DEVELOPMENT_TOOLS
        inline int32 GetSelectedOptionIndex() const { return m_selectedMaskIndex; }
        #endif

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;

        BoneMask const* GetBoneMaskForIndex( GraphContext& context, int32 optionIndex ) const;

        inline int32 TrySelectMask( GraphContext& context ) const
        {
            return VectorFindIndex( GetSettings<BoneMaskSelectorNode>()->m_parameterValues, m_pParameterValueNode->GetValue<StringID>( context ) );
        }

    private:

        ValueNodeID*                                        m_pParameterValueNode = nullptr;
        ValueNodeBoneMask*                                  m_pDefaultMaskValueNode = nullptr;
        TInlineVector<ValueNodeBoneMask*, 7>                m_boneMaskOptionNodes;
        int32                                               m_selectedMaskIndex = InvalidIndex;
        int32                                               m_newMaskIndex = InvalidIndex;
        BoneMask                                            m_blendedBoneMask;
        Seconds                                             m_currentTimeInBlend = 0;
        bool                                                m_isBlending = false;
    };
}