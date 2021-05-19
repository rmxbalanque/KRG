#include "AnimationGraphNode_RootMotionOverride.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void RootMotionOverrideNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        PassthroughNode::OnConstruct( pSettings, nodePtrs, dataSet );

        auto pNodeSettings = GetSettings<RootMotionOverrideNode>();
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_desiredHeadingVelocityNodeIdx, m_pDesiredHeadingVelocityNode );
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_desiredFacingDirectionNodeIdx, m_pDesiredFacingDirectionNode );
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_linearVelocityLimitNodeIdx, m_pLinearVelocityLimitNode );
        SetNodePtrFromIndex( nodePtrs, pNodeSettings->m_angularVelocityLimitNodeIdx, m_pAngularVelocityLimitNode );
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

    void RootMotionOverrideNode::ModifyDisplacement( GraphContext& context, UpdateResult& NodeResult ) const
    {
        auto pSettings = GetSettings<RootMotionOverrideNode>();
        KRG_ASSERT( context.IsValid() && pSettings != nullptr );

        //-------------------------------------------------------------------------

        Transform AdjustedDisplacementDelta = NodeResult.m_rootMotionDelta;

        // Heading
        //-------------------------------------------------------------------------

        bool isHeadingModificationAllowed = m_pDesiredHeadingVelocityNode != nullptr && pSettings->m_overrideFlags.AreAnyFlagsSet( OverrideFlags::HeadingX, OverrideFlags::HeadingY, OverrideFlags::HeadingZ );

        float maxLinearVelocity = pSettings->m_maxLinearVelocity;
        if ( m_pLinearVelocityLimitNode != nullptr )
        {
            maxLinearVelocity = Math::Abs( m_pLinearVelocityLimitNode->GetValue<float>( context ) * 100 );
            isHeadingModificationAllowed = !Math::IsNearZero( maxLinearVelocity );
        }

        if ( isHeadingModificationAllowed )
        {
            Vector const desiredHeadingVelocity = m_pDesiredHeadingVelocityNode->GetValue<Vector>( context );

            // Override the request axes with the desired heading
            Vector translation = NodeResult.m_rootMotionDelta.GetTranslation();
            translation.m_x = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingX ) ? desiredHeadingVelocity.m_x * context.m_deltaTime : translation.m_x;
            translation.m_y = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingY ) ? desiredHeadingVelocity.m_y * context.m_deltaTime : translation.m_y;
            translation.m_z = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::HeadingZ ) ? desiredHeadingVelocity.m_z * context.m_deltaTime : translation.m_z;

            // Apply max linear velocity limit
            float const maxLinearValue = context.m_deltaTime * maxLinearVelocity;
            if ( translation.GetLengthSquared3() > ( maxLinearValue * maxLinearValue ) )
            {
                translation.Normalize3();
                translation *= maxLinearValue;
            }

            AdjustedDisplacementDelta.SetTranslation( translation );
        }

        // Facing
        //-------------------------------------------------------------------------

        bool isFacingModificationAllowed = ( m_pDesiredFacingDirectionNode != nullptr ) && pSettings->m_overrideFlags.AreAnyFlagsSet( OverrideFlags::FacingX, OverrideFlags::FacingY, OverrideFlags::FacingZ );

        float maxAngularVelocity = pSettings->m_maxAngularVelocity;
        if ( m_pAngularVelocityLimitNode != nullptr )
        {
            maxAngularVelocity = Math::Abs( Math::DegreesToRadians * m_pAngularVelocityLimitNode->GetValue<float>( context ) );
            isFacingModificationAllowed = !Math::IsNearZero( maxAngularVelocity );
        }

        if ( isFacingModificationAllowed )
        {
            Vector desiredFacing = m_pDesiredFacingDirectionNode->GetValue<Vector>( context );
            desiredFacing.m_x = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingX ) ? desiredFacing.m_x * context.m_deltaTime : 0;
            desiredFacing.m_y = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingY ) ? desiredFacing.m_y * context.m_deltaTime : 0;
            desiredFacing.m_z = pSettings->m_overrideFlags.IsFlagSet( OverrideFlags::FacingZ ) ? desiredFacing.m_z * context.m_deltaTime : 0;

            if ( !desiredFacing.IsNearZero3() )
            {
                desiredFacing.Normalize3();

                // Get the total delta rotation between our current facing and the desired facing
                Quaternion deltaRotation = Quaternion::FromRotationBetweenNormalizedVectors( Vector::WorldForward, desiredFacing );

                // Apply max angular velocity limit
                float const maxAngularValue = context.m_deltaTime * maxAngularVelocity;
                float const desiredRotationAngle = deltaRotation.ToAxisAngle().m_angle;
                if ( desiredRotationAngle > maxAngularValue )
                {
                    float const T = 1.0f - ( ( desiredRotationAngle - maxAngularValue ) / desiredRotationAngle );
                    deltaRotation = Quaternion::SLerp( Quaternion::Identity, deltaRotation, T );
                }

                AdjustedDisplacementDelta.SetRotation( deltaRotation );
            }
        }

        //-------------------------------------------------------------------------

        NodeResult.m_rootMotionDelta = AdjustedDisplacementDelta;

        #if KRG_DEVELOPMENT_TOOLS
        context.GetRootMotionActionRecorder()->RecordModification( GetNodeIdx(), NodeResult.m_rootMotionDelta );
        #endif
    }

    UpdateResult RootMotionOverrideNode::Update( GraphContext& context )
    {
        UpdateResult Result = PassthroughNode::Update( context );
        ModifyDisplacement( context, Result );
        return Result;
    }

    UpdateResult RootMotionOverrideNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        UpdateResult Result = PassthroughNode::Update( context, updateRange );
        ModifyDisplacement( context, Result );
        return Result;
    }
}