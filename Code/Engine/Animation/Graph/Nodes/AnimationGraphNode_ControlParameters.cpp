#include "AnimationGraphNode_ControlParameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ControlParameterBool::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (bool*) pOutValue ) = m_value;
    }

    void ControlParameterBool::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(bool*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterID::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (StringID*) pOutValue ) = m_value;
    }

    void ControlParameterID::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(StringID*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterInt::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (int32*) pOutValue ) = m_value;
    }

    void ControlParameterInt::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(int32*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterFloat::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (float*) pOutValue ) = m_value;
    }

    void ControlParameterFloat::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(float*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterVector::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Vector*) pOutValue ) = m_value;
    }

    void ControlParameterVector::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Vector*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterTarget::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Target*) pOutValue ) = m_value;
    }

    void ControlParameterTarget::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Target*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterBoneMask::InitializeInternal( GraphContext& context )
    {
        ValueNodeBoneMask::InitializeInternal( context );
        m_value = eastl::move( BoneMask( context.GetSkeleton(), 1.0f ) ); // Default value sets a full mask i.e. has no effect
    }

    void ControlParameterBoneMask::ShutdownInternal( GraphContext& context )
    {
        m_value = eastl::move( BoneMask() );
        ValueNodeBoneMask::ShutdownInternal( context );
    }

    void ControlParameterBoneMask::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *reinterpret_cast<BoneMask const**>( pOutValue ) = &m_value;
    }

    void ControlParameterBoneMask::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        auto pBoneMaskPtr = *reinterpret_cast<BoneMask const* const*>( pInValue );
        m_value = *pBoneMaskPtr;
    }

    //-------------------------------------------------------------------------

    void VirtualParameterBool::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterBool>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterBool::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterBool::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void VirtualParameterBool::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<bool*>( pOutValue ) = m_pChildNode->GetValue<bool>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterID::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeID::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterID>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterID::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeID::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterID::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeID::ShutdownInternal( context );
    }

    void VirtualParameterID::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<StringID*>( pOutValue ) = m_pChildNode->GetValue<StringID>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterInt::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeInt::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterInt>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterInt::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeInt::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterInt::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeInt::ShutdownInternal( context );
    }

    void VirtualParameterInt::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<int32*>( pOutValue ) = m_pChildNode->GetValue<int32>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterFloat::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterFloat>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterFloat::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterFloat::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void VirtualParameterFloat::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<float*>( pOutValue ) = m_pChildNode->GetValue<float>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterVector::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeVector::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterVector>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterVector::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeVector::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterVector::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeVector::ShutdownInternal( context );
    }

    void VirtualParameterVector::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<Vector*>( pOutValue ) = m_pChildNode->GetValue<Vector>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterTarget::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeTarget::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterTarget>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterTarget::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeTarget::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterTarget::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeTarget::ShutdownInternal( context );
    }

    void VirtualParameterTarget::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<Target*>( pOutValue ) = m_pChildNode->GetValue<Target>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterBoneMask::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBoneMask::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VirtualParameterBoneMask>()->m_childNodeIdx, m_pChildNode );
    }

    void VirtualParameterBoneMask::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        ValueNodeBoneMask::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterBoneMask::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        ValueNodeBoneMask::ShutdownInternal( context );
    }

    void VirtualParameterBoneMask::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<BoneMask const**>( pOutValue ) = m_pChildNode->GetValue<BoneMask const*>( context );
    }
}