#include "AnimationGraphNode_ConstValues.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ConstValueBool::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueBool>( nodePtrs, options );
    }

    void ConstValueBool::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (bool*) pOutValue ) = GetSettings<ConstValueBool>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueID::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueID>( nodePtrs, options );
    }

    void ConstValueID::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (StringID*) pOutValue ) = GetSettings<ConstValueID>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueInt::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueInt>( nodePtrs, options );
    }

    void ConstValueInt::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (int32*) pOutValue ) = GetSettings<ConstValueInt>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueFloat::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueFloat>( nodePtrs, options );
    }

    void ConstValueFloat::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (float*) pOutValue ) = GetSettings<ConstValueFloat>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueVector::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueVector>( nodePtrs, options );
    }

    void ConstValueVector::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Vector*) pOutValue ) = GetSettings<ConstValueVector>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueTarget::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<ConstValueTarget>( nodePtrs, options );
    }

    void ConstValueTarget::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Target*) pOutValue ) = GetSettings<ConstValueTarget>()->m_value;
    }
}