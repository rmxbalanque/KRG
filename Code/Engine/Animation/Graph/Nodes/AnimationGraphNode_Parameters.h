#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    //-------------------------------------------------------------------------
    // Control Parameters
    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterBoolNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        bool m_value = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterIDNode final : public IDValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IDValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        StringID m_value;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterIntNode final : public IntValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IntValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        int32 m_value = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterFloatNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        float m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterVectorNode final : public VectorValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public VectorValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Vector m_value = Vector::Zero;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterTargetNode final : public TargetValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public TargetValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Target m_value;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterBoneMaskNode final : public BoneMaskValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoneMaskValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        BoneMask m_value;
    };

    //-------------------------------------------------------------------------
    // Virtual control parameters
    //-------------------------------------------------------------------------
    // NOTE: Virtual parameters are not allowed to cache values, as their result can change 

    class KRG_ENGINE_ANIMATION_API VirtualParameterBoolNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoolValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterIDNode final : public IDValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IDValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IDValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterIntNode final : public IntValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IntValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IntValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IntValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------
     
    class KRG_ENGINE_ANIMATION_API VirtualParameterFloatNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterVectorNode final : public VectorValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public VectorValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( VectorValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        VectorValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterTargetNode final : public TargetValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public TargetValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( TargetValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TargetValueNode* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterBoneMaskNode final : public BoneMaskValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoneMaskValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoneMaskValueNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoneMaskValueNode* m_pChildNode = nullptr;
    };
}