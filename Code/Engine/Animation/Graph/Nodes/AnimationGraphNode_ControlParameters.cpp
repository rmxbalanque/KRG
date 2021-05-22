#include "AnimationGraphNode_ControlParameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ControlParameterBool::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterBool>( nodePtrs, options );
    }

    void ControlParameterBool::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (bool*) pOutValue ) = m_value;
    }

    void ControlParameterBool::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(bool*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterID::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterID>( nodePtrs, options );
    }

    void ControlParameterID::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (StringID*) pOutValue ) = m_value;
    }

    void ControlParameterID::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(StringID*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterInt::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterInt>( nodePtrs, options );
    }

    void ControlParameterInt::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (int32*) pOutValue ) = m_value;
    }

    void ControlParameterInt::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(int32*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterFloat::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterFloat>( nodePtrs, options );
    }

    void ControlParameterFloat::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (float*) pOutValue ) = m_value;
    }

    void ControlParameterFloat::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(float*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterVector::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterVector>( nodePtrs, options );
    }

    void ControlParameterVector::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Vector*) pOutValue ) = m_value;
    }

    void ControlParameterVector::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Vector*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterTarget::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterTarget>( nodePtrs, options );
    }

    void ControlParameterTarget::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Target*) pOutValue ) = m_value;
    }

    void ControlParameterTarget::SetValueInternal( GraphContext& context, void const* pInValue )
    {
        m_value = *(Target*) pInValue;
    }

    //-------------------------------------------------------------------------

    void ControlParameterBoneMask::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ControlParameterBoneMask>( nodePtrs, options );
    }

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

    void VirtualParameterBool::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterBool>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterID::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterID>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterInt::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterInt>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterFloat::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterFloat>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterVector::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterVector>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterTarget::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterTarget>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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

    void VirtualParameterBoneMask::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VirtualParameterBoneMask>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_childNodeIdx, pNode->m_pChildNode );
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