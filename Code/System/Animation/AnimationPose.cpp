#include "AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        Pose::Pose( Skeleton const* pSkeleton, Init opt )
            : m_pSkeleton( pSkeleton )
            , m_localTransforms( pSkeleton->GetNumBones() )
            , m_isAdditive( false )
        {
            KRG_ASSERT( pSkeleton != nullptr );
            if ( opt == Init::ReferencePose )
            {
                SetReferencePose();
            }
        }

        void Pose::CopyFrom( Pose const* pRHS )
        {
            m_pSkeleton = pRHS->m_pSkeleton;
            m_localTransforms = pRHS->m_localTransforms;
            m_globalTransforms.clear();
            m_isAdditive = pRHS->m_isAdditive;
        }

        void Pose::SetReferencePose()
        {
            auto const& referencePose = m_pSkeleton->GetReferencePose();
            m_localTransforms = referencePose;
            m_globalTransforms.clear();
            m_isAdditive = false;
        }

        void Pose::SetZeroPose()
        {
            auto const& referencePose = m_pSkeleton->GetReferencePose();
            auto const numBones = m_pSkeleton->GetNumBones();
            m_localTransforms.resize( numBones, Transform::Identity );
            m_globalTransforms.clear();
            m_isAdditive = false;
        }

        void Pose::CalculateGlobalTransforms()
        {
            auto const numBones = m_pSkeleton->GetNumBones();
            m_globalTransforms.resize( numBones );

            m_globalTransforms[0] = m_localTransforms[0];
            for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
            {
                auto const parentIdx = m_pSkeleton->GetParentIndex( boneIdx );
                auto const parentTransform = m_globalTransforms[parentIdx];
                m_globalTransforms[boneIdx] = m_localTransforms[boneIdx] * parentTransform;
            }
        }

        Transform Pose::GetGlobalTransform( S32 boneIdx ) const
        {
            KRG_ASSERT( boneIdx < m_pSkeleton->GetNumBones() );

            Transform boneGlobalTransform;
            if ( !m_globalTransforms.empty() )
            {
                boneGlobalTransform = m_globalTransforms[boneIdx];
            }
            else
            {
                auto boneParents = KRG_STACK_ARRAY_ALLOC( S32, m_pSkeleton->GetNumBones() );
                S32 nextEntry = 0;

                // Get parent list
                S32 parentIdx = m_pSkeleton->GetParentIndex( boneIdx );
                while ( parentIdx != InvalidIndex )
                {
                    boneParents[nextEntry++] = parentIdx;
                    parentIdx = m_pSkeleton->GetParentIndex( parentIdx );
                }

                // If we have parents
                boneGlobalTransform = m_localTransforms[boneIdx];
                if ( nextEntry > 0 )
                {
                    // Calculate global transform of parent
                    S32 arrayIdx = nextEntry - 1;
                    parentIdx = boneParents[arrayIdx--];
                    auto parentGlobalTransform = m_localTransforms[parentIdx];
                    for ( arrayIdx; arrayIdx >= 0; arrayIdx-- )
                    {
                        S32 const nextIdx = boneParents[arrayIdx];
                        auto const nextTransform = m_localTransforms[nextIdx];
                        parentGlobalTransform = nextTransform * parentGlobalTransform;
                    }

                    // Calculate global transform of bone
                    boneGlobalTransform = boneGlobalTransform * parentGlobalTransform;
                }
            }

            return boneGlobalTransform;
        }
    }
}