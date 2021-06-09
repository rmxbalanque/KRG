#include "AnimationGraphNode_Math.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void FloatSwitchNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatSwitchNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_switchValueNodeIdx, pNode->m_pSwitchValueNode );
        SetNodePtrFromIndex( nodePtrs, m_trueValueNodeIdx, pNode->m_pTrueValueNode );
        SetNodePtrFromIndex( nodePtrs, m_falseValueNodeIdx, pNode->m_pFalseValueNode );
    }

    void FloatSwitchNode::InitializeInternal( GraphContext& context )
    {
        FloatValueNode::InitializeInternal( context );
        m_pSwitchValueNode->Initialize( context );
        m_pTrueValueNode->Initialize( context );
        m_pFalseValueNode->Initialize( context );
    }

    void FloatSwitchNode::ShutdownInternal( GraphContext& context )
    {
        m_pSwitchValueNode->Shutdown( context );
        m_pTrueValueNode->Shutdown( context );
        m_pFalseValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatSwitchNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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

    void TargetOffsetNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<TargetOffsetNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void TargetOffsetNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        TargetValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void TargetOffsetNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        TargetValueNode::ShutdownInternal( context );
    }

    void TargetOffsetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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

    void VectorNegateNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VectorNegateNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void VectorNegateNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        VectorValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorNegateNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        VectorValueNode::ShutdownInternal( context );
    }

    void VectorNegateNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( m_pInputValueNode != nullptr );

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            m_value = m_pInputValueNode->GetValue<Vector>( context ).GetNegated();
        }

        *reinterpret_cast<Vector*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatRemapNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatRemapNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void FloatRemapNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatRemapNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatRemapNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatRemapNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            float const inputValue = m_pInputValueNode->GetValue<float>( context );
            Percentage const percentageThroughInputRange( pSettings->m_inputRange.GetPercentageThroughClamped( inputValue ) );

            if ( pSettings->m_outputRange.IsValid() )
            {
                m_value = pSettings->m_outputRange.GetValueForPercentageThroughClamped( percentageThroughInputRange );
            }
            else
            {
                FloatRange const validRange( pSettings->m_outputRange.m_end, pSettings->m_outputRange.m_start );
                m_value = validRange.GetValueForPercentageThroughClamped( 1.0f - percentageThroughInputRange );
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatClampNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatClampNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void FloatClampNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatClampNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatClampNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatClampNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            auto const inputValue = m_pInputValueNode->GetValue<float>( context );
            m_value = pSettings->m_clampRange.GetClampedValue( inputValue );
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void FloatAbsNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatAbsNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void FloatAbsNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void FloatAbsNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatAbsNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatAbsNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            auto const inputValue = m_pInputValueNode->GetValue<float>( context );
            m_value = Math::Abs( inputValue );
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void IDToFloatNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<IDToFloatNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void IDToFloatNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );

        auto pSettings = GetSettings<IDToFloatNode>();
        KRG_ASSERT( pSettings->m_IDs.size() == pSettings->m_values.size() );
    }

    void IDToFloatNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void IDToFloatNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IDToFloatNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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

    void VectorInfoNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<VectorInfoNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void VectorInfoNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorInfoNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void VectorInfoNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<VectorInfoNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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

    void FloatBlendNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatBlendNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void FloatBlendNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );

        m_blendRange = FloatRange( m_pInputValueNode->GetValue<float>( context ) );
        m_currentValue = m_blendRange.m_end;
        m_currentBlendTime = 0;
    }

    void FloatBlendNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatBlendNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatBlendNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            float const inputTargetValue = m_pInputValueNode->GetValue<float>( context );
            if ( Math::IsNearEqual( m_currentValue, inputTargetValue, 0.01f ) )
            {
                m_blendRange = FloatRange( inputTargetValue );
                m_currentValue = inputTargetValue;
                m_currentBlendTime = 0;
            }
            else
            {
                // If the target has changed
                if ( inputTargetValue != m_blendRange.m_end )
                {
                    m_blendRange.m_end = inputTargetValue;
                    m_blendRange.m_start = m_currentValue;
                    m_currentBlendTime = 0;
                }

                // Increment the time through the blend
                m_currentBlendTime += context.m_deltaTime;

                // Calculate the new value, based on the percentage through the blend calculated by the easing function
                float const T = Math::Clamp( m_currentBlendTime / pSettings->m_blendTime, 0.0f, 1.0f );
                float const blendValue = Math::Easing::EvaluateEasingFunction( pSettings->m_blendType, T ) * m_blendRange.GetLength();
                m_currentValue = m_blendRange.m_start + blendValue;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_currentValue;
    }

    //-------------------------------------------------------------------------

    void FloatMathNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatMathNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_valueNodeIdxA, pNode->m_pValueNodeA );
        SetNodePtrFromIndex( nodePtrs, m_valueNodeIdxB, pNode->m_pValueNodeB );
    }

    void FloatMathNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pValueNodeA != nullptr );
        FloatValueNode::InitializeInternal( context );
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
        FloatValueNode::ShutdownInternal( context );
    }

    void FloatMathNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pValueNodeA != nullptr );
        auto FloatNodeSettings = GetSettings<FloatMathNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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

    void TargetInfoNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<TargetInfoNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputTargetNodeIdx, pNode->m_pTargetNode );
    }

    void TargetInfoNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pTargetNode->Initialize( context );
    }

    void TargetInfoNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        m_pTargetNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void TargetInfoNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pTargetNode != nullptr );
        auto pSettings = GetSettings<TargetInfoNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

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