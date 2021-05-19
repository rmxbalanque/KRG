#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    //-------------------------------------------------------------------------
    // Control Parameters
    //-------------------------------------------------------------------------

    class ControlParameterBool : public ValueNodeBool
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        bool m_value = false;
    };

    //-------------------------------------------------------------------------

    class ControlParameterID : public ValueNodeID
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        StringID m_value;
    };

    //-------------------------------------------------------------------------

    class ControlParameterInt : public ValueNodeInt
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        int32 m_value = 0;
    };

    //-------------------------------------------------------------------------

    class ControlParameterFloat : public ValueNodeFloat
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        float m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class ControlParameterVector : public ValueNodeVector
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Vector m_value = Vector::Zero;
    };

    //-------------------------------------------------------------------------

    class ControlParameterTarget : public ValueNodeTarget
    {
    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Target m_value;
    };

    //-------------------------------------------------------------------------

    class ControlParameterBoneMask : public ValueNodeBoneMask
    {

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

    class VirtualParameterBool : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBool* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterID : public ValueNodeID
    {
    public:

        struct Settings : public ValueNodeID::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterInt : public ValueNodeInt
    {
    public:

        struct Settings : public ValueNodeInt::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeInt* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------
     
    class VirtualParameterFloat : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterVector : public ValueNodeVector
    {
    public:

        struct Settings : public ValueNodeVector::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterTarget : public ValueNodeTarget
    {
    public:

        struct Settings : public ValueNodeTarget::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeTarget* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterBoneMask : public ValueNodeBoneMask
    {
    public:

        struct Settings : public ValueNodeBoneMask::Settings
        {
            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBoneMask* m_pChildNode = nullptr;
    };
}