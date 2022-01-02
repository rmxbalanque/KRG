#include "Animation_RuntimeGraphNode_Parameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void ControlParameterBoolNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterBoolNode>( nodePtrs, options );
    }

    void ControlParameterBoolNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (bool*) pOutValue ) = m_value;
    }

    void ControlParameterBoolNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(bool*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterIDNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterIDNode>( nodePtrs, options );
    }

    void ControlParameterIDNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (StringID*) pOutValue ) = m_value;
    }

    void ControlParameterIDNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(StringID*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterIntNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterIntNode>( nodePtrs, options );
    }

    void ControlParameterIntNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (int32*) pOutValue ) = m_value;
    }

    void ControlParameterIntNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(int32*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterFloatNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterFloatNode>( nodePtrs, options );
    }

    void ControlParameterFloatNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (float*) pOutValue ) = m_value;
    }

    void ControlParameterFloatNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(float*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterVectorNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterVectorNode>( nodePtrs, options );
    }

    void ControlParameterVectorNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Vector*) pOutValue ) = m_value;
    }

    void ControlParameterVectorNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Vector*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterTargetNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterTargetNode>( nodePtrs, options );
    }

    void ControlParameterTargetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Target*) pOutValue ) = m_value;
    }

    void ControlParameterTargetNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Target*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterBoneMaskNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterBoneMaskNode>( nodePtrs, options );
    }

    void ControlParameterBoneMaskNode::InitializeInternal( GraphContext& context )
    {
        BoneMaskValueNode::InitializeInternal( context );
        m_value = eastl::move( BoneMask( context.m_pSkeleton, 1.0f ) ); // Default value sets a full mask i.e. has no effect
    }

    void ControlParameterBoneMaskNode::ShutdownInternal( GraphContext& context )
    {
        m_value = eastl::move( BoneMask() );
        BoneMaskValueNode::ShutdownInternal( context );
    }

    void ControlParameterBoneMaskNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *reinterpret_cast<BoneMask const**>( pOutValue ) = &m_value;
    }

    void ControlParameterBoneMaskNode::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        auto pBoneMaskPtr = *reinterpret_cast<BoneMask const* const*>( pInValue );
        m_value = *pBoneMaskPtr;
    }

    //-------------------------------------------------------------------------

    void VirtualParameterBoolNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterBoolNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterBoolNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterBoolNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void VirtualParameterBoolNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<bool*>( pOutValue ) = m_pChildNode->GetValue<bool>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterIDNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterIDNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterIDNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        IDValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterIDNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        IDValueNode::ShutdownInternal( context );
    }

    void VirtualParameterIDNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<StringID*>( pOutValue ) = m_pChildNode->GetValue<StringID>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterIntNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterIntNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterIntNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        IntValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterIntNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        IntValueNode::ShutdownInternal( context );
    }

    void VirtualParameterIntNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<int32*>( pOutValue ) = m_pChildNode->GetValue<int32>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterFloatNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterFloatNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterFloatNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterFloatNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void VirtualParameterFloatNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<float*>( pOutValue ) = m_pChildNode->GetValue<float>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterVectorNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterVectorNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterVectorNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        VectorValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterVectorNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        VectorValueNode::ShutdownInternal( context );
    }

    void VirtualParameterVectorNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<Vector*>( pOutValue ) = m_pChildNode->GetValue<Vector>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterTargetNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterTargetNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterTargetNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        TargetValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterTargetNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        TargetValueNode::ShutdownInternal( context );
    }

    void VirtualParameterTargetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<Target*>( pOutValue ) = m_pChildNode->GetValue<Target>( context );
    }

    //-------------------------------------------------------------------------

    void VirtualParameterBoneMaskNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterBoneMaskNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
    }

    void VirtualParameterBoneMaskNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        BoneMaskValueNode::InitializeInternal( context );
        m_pChildNode->Initialize( context );
    }

    void VirtualParameterBoneMaskNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        m_pChildNode->Shutdown( context );
        BoneMaskValueNode::ShutdownInternal( context );
    }

    void VirtualParameterBoneMaskNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pChildNode != nullptr );
        *reinterpret_cast<BoneMask const**>( pOutValue ) = m_pChildNode->GetValue<BoneMask const*>( context );
    }
}