#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API BoneMaskNode final : public BoneMaskValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoneMaskValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoneMaskValueNode::Settings, m_rootMotionWeight, m_weights );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

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

    class KRG_ENGINE_ANIMATION_API BoneMaskBlendNode final : public BoneMaskValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoneMaskValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoneMaskValueNode::Settings, m_sourceMaskNodeIdx, m_targetMaskNodeIdx, m_blendWeightValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                           m_sourceMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_targetMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_blendWeightValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoneMaskValueNode*                      m_pSourceBoneMask = nullptr;
        BoneMaskValueNode*                      m_pTargetBoneMask = nullptr;
        FloatValueNode*                         m_pBlendWeightValueNode = nullptr;
        BoneMask                                m_blendedBoneMask;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoneMaskSelectorNode final : public BoneMaskValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoneMaskValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoneMaskValueNode::Settings, m_defaultMaskNodeIdx, m_parameterValueNodeIdx, m_switchDynamically, m_maskNodeIndices, m_parameterValues, m_blendTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

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

        BoneMask const* GetBoneMaskForIndex( GraphContext& context, int32 optionIndex ) const;

        inline int32 TrySelectMask( GraphContext& context ) const
        {
            return VectorFindIndex( GetSettings<BoneMaskSelectorNode>()->m_parameterValues, m_pParameterValueNode->GetValue<StringID>( context ) );
        }

    private:

        IDValueNode*                                        m_pParameterValueNode = nullptr;
        BoneMaskValueNode*                                  m_pDefaultMaskValueNode = nullptr;
        TInlineVector<BoneMaskValueNode*, 7>                m_boneMaskOptionNodes;
        int32                                               m_selectedMaskIndex = InvalidIndex;
        int32                                               m_newMaskIndex = InvalidIndex;
        BoneMask                                            m_blendedBoneMask;
        Seconds                                             m_currentTimeInBlend = 0;
        bool                                                m_isBlending = false;
    };
}