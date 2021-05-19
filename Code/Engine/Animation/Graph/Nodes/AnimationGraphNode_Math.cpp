#include "AnimationGraphNode_Math.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void FloatSwitchNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        auto pNodeSettings = GetSettings<FloatSwitchNode>();
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_switchValueNodeIdx, m_pSwitchValueNode );
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_trueValueNodeIdx, m_pTrueValueNode );
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_falseValueNodeIdx, m_pFalseValueNode );
    }

    void FloatSwitchNode::InitializeInternal( GraphContext& context )
    {
        ValueNodeFloat::InitializeInternal( context );
        m_pSwitchValueNode->Initialize( context );
        m_pTrueValueNode->Initialize( context );
        m_pFalseValueNode->Initialize( context );
    }

    void FloatSwitchNode::ShutdownInternal( GraphContext& context )
    {
        m_pSwitchValueNode->Shutdown( context );
        m_pTrueValueNode->Shutdown( context );
        m_pFalseValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatSwitchNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            if ( m_pSwitchValueNode->GetValue<bool>( context ) )
            {
                m_value = m_pTrueValueNode->GetValue<float>( context );
            }
            else
            {
                m_value = m_pFalseValueNode->GetValue<float>( context );
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void TargetOffsetNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeTarget::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<TargetOffsetNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void TargetOffsetNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        ValueNodeTarget::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void TargetOffsetNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeTarget::ShutdownInternal( context );
    }

    void TargetOffsetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            m_value = m_pInputValueNode->GetValue<Target>( context );

            if ( m_value.IsTargetSet() )
            {
                auto pSettings = GetSettings<TargetOffsetNode>();
                m_value.SetOffsets( pSettings->m_rotationOffset, pSettings->m_translationOffset, pSettings->m_isBoneSpaceOffset );
            }
            else
            {
                KRG_LOG_WARNING( "Animation", "Trying to set an offset on an unset node!" );
            }
        }

        *reinterpret_cast<Target*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void VectorNegateNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeVector::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VectorNegateNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void VectorNegateNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        ValueNodeVector::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorNegateNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeVector::ShutdownInternal( context );
    }

    void VectorNegateNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            m_value = m_pInputValueNode->GetValue<Vector>( context ).GetNegated();
        }

        *reinterpret_cast<Vector*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatRemapNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatRemapNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void FloatRemapNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatRemapNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatRemapNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatRemapNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            float const inputValue = m_pInputValueNode->GetValue<float>( context );
            Percentage const percentageThroughInputRange( pSettings->m_inputRange.GetPercentageThroughClamped( inputValue ) );

            if ( pSettings->m_outputRange.IsValid() )
            {
                m_value = pSettings->m_outputRange.GetValueForPercentageThroughClamped( percentageThroughInputRange );
            }
            else
            {
                TRange<float> const validRange( pSettings->m_outputRange.m_max, pSettings->m_outputRange.m_min );
                m_value = validRange.GetValueForPercentageThroughClamped( 1.0f - percentageThroughInputRange );
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatClampNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatClampNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void FloatClampNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatClampNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatClampNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto FloatNodeSettings = GetSettings<FloatClampNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            auto const inputValue = m_pInputValueNode->GetValue<float>( context );
            m_value = Math::Clamp( inputValue, FloatNodeSettings->m_minValue, FloatNodeSettings->m_maxValue );
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatAbsNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatAbsNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void FloatAbsNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatAbsNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatAbsNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatAbsNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            auto const inputValue = m_pInputValueNode->GetValue<float>( context );
            m_value = Math::Abs( inputValue );
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void IDToFloatNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<IDToFloatNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void IDToFloatNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );

        auto pSettings = GetSettings<IDToFloatNode>();
        KRG_ASSERT( pSettings->m_IDs.size() == pSettings->m_values.size() );
    }

    void IDToFloatNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void IDToFloatNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IDToFloatNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            StringID const inputID = m_pInputValueNode->GetValue<StringID>( context );
            int32 const foundIdx = VectorFindIndex( pSettings->m_IDs, inputID );
            if ( foundIdx != InvalidIndex )
            {
                m_value = pSettings->m_values[foundIdx];
            }
            else
            {
                m_value = pSettings->m_defaultValue;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void VectorInfoNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<VectorInfoNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void VectorInfoNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorInfoNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void VectorInfoNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<VectorInfoNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            Vector const inputVector = m_pInputValueNode->GetValue<Vector>( context );
            switch ( pSettings->m_desiredInfo )
            {
                case Info::X: m_value = inputVector.m_x; break;
                case Info::Y: m_value = inputVector.m_y; break;
                case Info::Z: m_value = inputVector.m_z; break;
                case Info::W: m_value = inputVector.m_w; break;

                case Info::Length:
                {
                    m_value = inputVector.GetLength3();
                }
                break;

                case Info::AngleHorizontal:
                {
                    m_value = Math::CalculateHorizontalAngleBetweenVectors( Vector::WorldForward, inputVector ).ToFloat();
                }
                break;

                case Info::AngleVertical:
                {
                    m_value = Math::CalculateVerticalAngleBetweenVectors( Vector::WorldForward, inputVector ).ToFloat();
                }
                break;

                case Info::SizeHorizontal:
                {
                    m_value = inputVector.GetLength3();
                }
                break;

                case Info::SizeVertical:
                {
                    m_value = Vector( inputVector.m_x, 0.0f, inputVector.m_z ).GetLength3();
                }
                break;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatBlendNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatBlendNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void FloatBlendNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        ValueNodeFloat::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );

        m_blendRange = TRange<float>( m_pInputValueNode->GetValue<float>( context ) );
        m_currentValue = m_blendRange.m_max;
        m_currentBlendTime = 0;
    }

    void FloatBlendNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatBlendNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatBlendNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            float const inputTargetValue = m_pInputValueNode->GetValue<float>( context );
            if ( Math::IsNearEqual( m_currentValue, inputTargetValue, 0.01f ) )
            {
                m_blendRange = TRange<float>( inputTargetValue );
                m_currentValue = inputTargetValue;
                m_currentBlendTime = 0;
            }
            else
            {
                // If the target has changed
                if ( inputTargetValue != m_blendRange.m_max )
                {
                    m_blendRange.m_max = inputTargetValue;
                    m_blendRange.m_min = m_currentValue;
                    m_currentBlendTime = 0;
                }

                // Increment the time through the blend
                m_currentBlendTime += context.m_deltaTime;

                // Calculate the new value, based on the percentage through the blend calculated by the easing function
                float const T = Math::Clamp( m_currentBlendTime / pSettings->m_blendTime, 0.0f, 1.0f );
                float const blendValue = Math::Easing::EvaluateEasingFunction( pSettings->m_blendType, T ) * ( m_blendRange.m_max - m_blendRange.m_min );
                m_currentValue = m_blendRange.m_min + blendValue;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_currentValue;
    }

    //-------------------------------------------------------------------------

    void FloatMathNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatMathNode>()->m_valueNodeIdxA, m_pValueNodeA );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatMathNode>()->m_valueNodeIdxB, m_pValueNodeB );
    }

    void FloatMathNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pValueNodeA != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pValueNodeA->Initialize( context );

        if ( m_pValueNodeB != nullptr )
        {
            m_pValueNodeB->Initialize( context );
        }
    }

    void FloatMathNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pValueNodeA != nullptr );
        if ( m_pValueNodeB != nullptr )
        {
            m_pValueNodeB->Shutdown( context );
        }

        m_pValueNodeA->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void FloatMathNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pValueNodeA != nullptr );
        auto FloatNodeSettings = GetSettings<FloatMathNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            // Get A
            float const valueA = m_pValueNodeA->GetValue<float>( context );

            // Get B
            float valueB;
            if ( m_pValueNodeB != nullptr )
            {
                valueB = m_pValueNodeB->GetValue<float>( context );
            }
            else
            {
                valueB = FloatNodeSettings->m_valueB;
            }

            // Calculate Result
            switch ( FloatNodeSettings->m_operator )
            {
                case Operator::Add:
                {
                    m_value = valueA + valueB;
                }
                break;

                case Operator::Sub:
                {
                    m_value = valueA - valueB;
                }
                break;

                case Operator::Mul:
                {
                    m_value = valueA * valueB;
                }
                break;

                case Operator::Div:
                {
                    if ( Math::IsNearZero( valueB ) )
                    {
                        KRG_LOG_WARNING( "Animation", "Dividing by zero in FloatMathNode" );
                        m_value = FLT_MAX;
                    }
                    else
                    {
                        m_value = valueA / valueB;
                    }
                }
                break;
            }

            //-------------------------------------------------------------------------

            if ( FloatNodeSettings->m_returnAbsoluteResult )
            {
                m_value = Math::Abs( m_value );
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void TargetInfoNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeFloat::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<TargetInfoNode>()->m_inputTargetNodeIdx, m_pTargetNode );
    }

    void TargetInfoNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        ValueNodeFloat::InitializeInternal( context );
        m_pTargetNode->Initialize( context );
    }

    void TargetInfoNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        m_pTargetNode->Shutdown( context );
        ValueNodeFloat::ShutdownInternal( context );
    }

    void TargetInfoNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        auto pSettings = GetSettings<TargetInfoNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

            Target const inputTarget = m_pTargetNode->GetValue<Target>( context );
            if ( inputTarget.IsTargetSet() )
            {
                // Get the transform out of the source target
                Transform inputTargetTransform;
                bool bIsValidTransform = inputTarget.TryGetTransform( context.m_pPreviousPose, inputTargetTransform );

                // If we have a valid transform, update the internal value
                if ( bIsValidTransform )
                {
                    bool const isInWorldSpace = ( pSettings->m_expectedCoordinateSpace == CoordinateSpace::World );
                    if ( isInWorldSpace )
                    {
                        inputTargetTransform = inputTargetTransform * context.m_worldTransformInverse;
                    }

                    //-------------------------------------------------------------------------
                    // NB: Target transform is in character space

                    switch ( pSettings->m_infoType )
                    {
                        case Info::AngleHorizontal:
                        {
                            Vector const direction = inputTargetTransform.GetTranslation().GetNormalized2();
                            if ( direction.IsNearZero3() )
                            {
                                m_value = 0.0f;
                            }
                            else
                            {
                                float const dotForward = Vector::WorldForward.GetDot3( direction );
                                m_value = Math::ACos( dotForward );
                                m_value = Math::RadiansToDegrees * m_value;

                                float const dotRight = Vector::WorldRight.GetDot3( direction );
                                if ( dotRight < 0.0f )
                                {
                                    m_value = -m_value;
                                }
                            }
                        }
                        break;

                        //-------------------------------------------------------------------------

                        case Info::AngleVertical:
                        {
                            Vector const Direction = inputTargetTransform.GetTranslation().GetNormalized3();
                            if ( Direction.IsNearZero3() )
                            {
                                m_value = 0.0f;
                            }
                            else
                            {
                                m_value = Math::RadiansToDegrees * ( Math::PiDivTwo - Math::ACos( Vector::WorldUp.GetDot3( Direction ) ) );
                            }
                        }
                        break;

                        //-------------------------------------------------------------------------

                        case Info::Distance:
                        {
                            m_value = inputTargetTransform.GetTranslation().GetLength3();
                        }
                        break;

                        //-------------------------------------------------------------------------

                        case Info::DistanceHorizontalOnly:
                        {
                            m_value = inputTargetTransform.GetTranslation().GetLength2();
                        }
                        break;

                        //-------------------------------------------------------------------------

                        case Info::DistanceVerticalOnly:
                        {
                            m_value = Math::Abs( inputTargetTransform.GetTranslation().m_z );
                        }
                        break;

                        //-------------------------------------------------------------------------

                        case Info::DeltaOrientationX:
                        {
                            m_value = inputTargetTransform.GetRotation().ToEulerAngles().m_x;
                        }
                        break;

                        case Info::DeltaOrientationY:
                        {
                            m_value = inputTargetTransform.GetRotation().ToEulerAngles().m_y;
                        }
                        break;

                        case Info::DeltaOrientationZ:
                        {
                            m_value = inputTargetTransform.GetRotation().ToEulerAngles().m_z;
                        }
                        break;
                    }
                }
            }
            else
            {
                m_value = 0.0f;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }
}