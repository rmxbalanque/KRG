#include "AnimationGraphNode_ConstValues.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ConstValueBool::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (bool*) pOutValue ) = GetSettings<ConstValueBool>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueID::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (StringID*) pOutValue ) = GetSettings<ConstValueID>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueInt::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (int32*) pOutValue ) = GetSettings<ConstValueInt>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueFloat::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (float*) pOutValue ) = GetSettings<ConstValueFloat>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueVector::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Vector*) pOutValue ) = GetSettings<ConstValueVector>()->m_value;
    }

    //-------------------------------------------------------------------------

    void ConstValueTarget::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        *( (Target*) pOutValue ) = GetSettings<ConstValueTarget>()->m_value;
    }
}