#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API AndNode final : public BoolValueNode
    {

    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            TInlineVector<NodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<BoolValueNode*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API OrNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            TInlineVector<NodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<BoolValueNode*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API NotNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoolValueNode*                              m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatComparisonNode final : public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            GreaterThanEqual = 0,
            LessThanEqual,
            NearEqual,
            GreaterThan,
            LessThan,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparandValueNodeIdx, m_comparison, m_epsilon, m_comparisonValue );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            NodeIndex                               m_comparandValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::GreaterThanEqual;
            float                                   m_epsilon = 0.0f;
            float                                   m_comparisonValue = 0.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                             m_pInputValueNode = nullptr;
        FloatValueNode*                             m_pComparandValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API RangeComparisonNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_range );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            FloatRange                              m_range;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                             m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API IDComparisonNode final: public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparand, m_comparison );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            StringID                                m_comparand;
            Comparison                              m_comparison = Comparison::Matches;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API MultipleIDComparisonNode final : public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparison, m_comparisionIDs );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::Matches;
            TInlineVector<StringID, 4>              m_comparisionIDs;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API IsTargetSetNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TargetValueNode*                m_pInputValueNode = nullptr;
        bool                            m_result = false;
    };
}