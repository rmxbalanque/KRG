#include "AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        bool Skeleton::IsValid() const
        {
            return !m_boneIDs.empty() && ( m_boneIDs.size() == m_parentMap.size() ) && ( m_boneIDs.size() == m_referencePose.size() );
        }

        Transform Skeleton::GetBoneGlobalTransform( S32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_referencePose.size() );

            Transform boneGlobalTransform = m_referencePose[idx];
            S32 parentIdx = GetParentIndex( idx );

            while ( parentIdx != InvalidIndex )
            {
                boneGlobalTransform = boneGlobalTransform * m_referencePose[parentIdx];
                parentIdx = GetParentIndex( parentIdx );
            }

            return boneGlobalTransform;
        }
    }
}