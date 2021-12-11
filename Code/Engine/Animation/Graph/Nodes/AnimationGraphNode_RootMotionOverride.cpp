#include "AnimationGraphNode_RootMotionOverride.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void RootMotionOverrideNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<RootMotionOverrideNode>( nodePtrs, options );
        SetOptionalNodePtrFromIndex( nodePtrs, m_desiredHeadingVelocityNodeIdx, pNode->m_pDesiredHeadingVelocityNode );
        SetOptionalNodePtrFromIndex( nodePtrs, m_desiredFacingDirectionNodeIdx, pNode->m_pDesiredFacingDirectionNode );
        SetOptionalNodePtrFromIndex( nodePtrs, m_linearVelocityLimitNodeIdx, pNode->m_pLinearVelocityLimitNode );
        SetOptionalNodePtrFromIndex( nodePtrs, m_angularVelocityLimitNodeIdx, pNode->m_pAngularVelocityLimitNode );
        PassthroughNode::Settings::InstantiateNode( nodePtrs, pDataSet, GraphNode::Settings::InitOptions::OnlySetPointers );
    }

    void RootMotionOverrideNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() && ( m_pDesiredHeadingVelocityNode != nullptr || m_pDesiredFacingDirectionNode != nullptr ) );

        PassthroughNode::InitializeInternal( context, initialTime );

        if ( m_pDesiredHeadingVelocityNode != nullptr )
        {
            m_pDesiredHeadingVelocityNode->Initialize( context );
        }

        if ( m_pDesiredFacingDirectionNode != nullptr )
        {
            m_pDesiredFacingDirectionNode->Initialize( context );
        }

        if ( m_pAngularVelocityLimitNode != nullptr )
        {
            m_pAngularVelocityLimitNode->Initialize( context );
        }

        if ( m_pLinearVelocityLimitNode != nullptr )
        {
            m_pLinearVelocityLimitNode->Initialize( context );
        }
    }

    void RootMotionOverrideNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && ( m_pDesiredHeadingVelocityNode != nullptr || m_pDesiredFacingDirectionNode != nullptr ) );

        if ( m_pDesiredHeadingVelocityNode != nullptr )
        {
            m_pDesiredHeadingVelocityNode->Shutdown( context );
        }

        if ( m_pDesiredFacingDirectionNode != nullptr )
        {
            m_pDesiredFacingDirectionNode->Shutdown( context );
        }

        if ( m_pAngularVelocityLimitNode != nullptr )
        {
            m_pAngularVelocityLimitNode->Shutdown( context );
        }

        if ( m_pLinearVelocityLimitNode != nullptr )
        {
            m_pLinearVelocityLimitNode->Shutdown( context );
        }

        PassthroughNode::ShutdownInternal( context );
    }

    void RootMotionOverrideNode::ModifyDisplacement( GraphContext& context, PoseNodeResult& NodeResult ) const
    {
        auto pSettings = GetSettings<RootMotionOverrideNode>();
        KRG_ASSERT( context.IsValid() && pSettings != nullptr );

        //-------------------------------------------------------------------------

        Transform AdjustedDisplacementDelta = NodeResult.m_rootMotionDelta;

        // Heading
        //-------------------------------------------------------------------------

        bool isHeadingModificationAllowed = m_pDesiredHeadingVelocityNode != nullptr && pSettings->m_overrideFlags.AreAnyFlagsSet( OverrideFlags::HeadingX, OverrideFlags::HeadingY, OverrideFlags::HeadingZ );
        if ( isHeadingModificationAllowed )
        {
            Vector const desiredHeadingVelocity = m_pDesiredHeadingVelocityNode->GetValue<Vector>( context );

            // Override the request axes with the desired heading
            Vector translation = NodeResult.m_rootMotionDelta.GetTranslation();
            translation.m_x = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingX ) ? desiredHeadingVelocity.m_x * context.m_deltaTime : translation.m_x;
            translation.m_y = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingY ) ? desiredHeadingVelocity.m_y * context.m_deltaTime : translation.m_y;
            translation.m_z = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingZ ) ? desiredHeadingVelocity.m_z * context.m_deltaTime : translation.m_z;

            // Apply max linear velocity limit
            float maxLinearVelocity = pSettings->m_maxLinearVelocity;
            if ( m_pLinearVelocityLimitNode != nullptr )
            {
                maxLinearVelocity = Math::Abs( m_pLinearVelocityLimitNode->GetValue<float>( context ) * 100 );
            }

            if ( maxLinearVelocity >= 0 )
            {
                float const maxLinearValue = context.m_deltaTime * maxLinearVelocity;
                if ( translation.GetLengthSquared3() > ( maxLinearValue * maxLinearValue ) )
                {
                    translation.Normalize3();
                    translation *= maxLinearValue;
                }
            }

            AdjustedDisplacementDelta.SetTranslation( translation );
        }

        // Facing
        //-------------------------------------------------------------------------

        bool isFacingModificationAllowed = ( m_pDesiredFacingDirectionNode != nullptr ) && pSettings->m_overrideFlags.AreAnyFlagsSet( OverrideFlags::FacingX, OverrideFlags::FacingY, OverrideFlags::FacingZ );
        if ( isFacingModificationAllowed )
        {
            Vector desiredFacingCS = m_pDesiredFacingDirectionNode->GetValue<Vector>( context );
            desiredFacingCS.m_x = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingX ) ? desiredFacingCS.m_x * context.m_deltaTime : 0;
            desiredFacingCS.m_y = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingY ) ? desiredFacingCS.m_y * context.m_deltaTime : 0;
            desiredFacingCS.m_z = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingZ ) ? desiredFacingCS.m_z * context.m_deltaTime : 0;

            if ( !desiredFacingCS.IsNearZero3() )
            {
                desiredFacingCS.Normalize3();

                // Get the total delta rotation between our current facing and the desired facing
                Quaternion deltaRotation = Quaternion::LookAt( Vector::WorldForward, desiredFacingCS, Vector::UnitZ );

                // Apply max angular velocity limit
                float maxAngularVelocity = pSettings->m_maxAngularVelocity;
                if ( m_pAngularVelocityLimitNode != nullptr )
                {
                    maxAngularVelocity = Math::Abs( Math::DegreesToRadians * m_pAngularVelocityLimitNode->GetValue<float>( context ) );
                }

                if ( maxAngularVelocity >= 0 )
                {
                    float const maxAngularValue = context.m_deltaTime * maxAngularVelocity;
                    float const desiredRotationAngle = (float) deltaRotation.ToAxisAngle().m_angle;
                    if (  desiredRotationAngle > maxAngularValue )
                    {
                        float const T = 1.0f - ( ( desiredRotationAngle - maxAngularValue ) / desiredRotationAngle );
                        deltaRotation = Quaternion::SLerp( Quaternion::Identity, deltaRotation, T );
                    }
                }

                AdjustedDisplacementDelta.SetRotation( deltaRotation );
            }
        }

        //-------------------------------------------------------------------------

        NodeResult.m_rootMotionDelta = AdjustedDisplacementDelta;

        #if KRG_DEVELOPMENT_TOOLS
        context.GetRootMotionActionRecorder()->RecordModification( GetNodeIndex(), NodeResult.m_rootMotionDelta );
        #endif
    }

    PoseNodeResult RootMotionOverrideNode::Update( GraphContext& context )
    {
        PoseNodeResult Result = PassthroughNode::Update( context );
        ModifyDisplacement( context, Result );
        return Result;
    }

    PoseNodeResult RootMotionOverrideNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        PoseNodeResult Result = PassthroughNode::Update( context, updateRange );
        ModifyDisplacement( context, Result );
        return Result;
    }
}