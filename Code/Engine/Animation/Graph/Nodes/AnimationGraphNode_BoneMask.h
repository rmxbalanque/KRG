#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API BoneMaskNode final : public ValueNodeBoneMask
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBoneMask::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBoneMask::Settings, m_rootMotionWeight, m_weights );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

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

    class KRG_ENGINE_ANIMATION_API BoneMaskBlendNode final : public ValueNodeBoneMask
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBoneMask::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBoneMask::Settings, m_sourceMaskNodeIdx, m_targetMaskNodeIdx, m_blendWeightValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                           m_sourceMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_targetMaskNodeIdx = InvalidIndex;
            NodeIndex                           m_blendWeightValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBoneMask*                      m_pSourceBoneMask = nullptr;
        ValueNodeBoneMask*                      m_pTargetBoneMask = nullptr;
        ValueNodeFloat*                         m_pBlendWeightValueNode = nullptr;
        BoneMask                                m_blendedBoneMask;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoneMaskSelectorNode final : public ValueNodeBoneMask
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBoneMask::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBoneMask::Settings, m_defaultMaskNodeIdx, m_parameterValueNodeIdx, m_switchDynamically, m_maskNodeIndices, m_parameterValues, m_blendTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

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