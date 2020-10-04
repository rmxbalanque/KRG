#include "SkeletalMesh.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool SkeletalMesh::IsValid() const
        {
            return m_indexBuffer.IsValid() && m_vertexBuffer.IsValid() && ( m_boneIDs.size() == m_parentBoneIndices.size() ) && ( m_boneIDs.size() == m_bindPose.size() );
        }

        S32 SkeletalMesh::GetBoneIndex( StringID const& boneID ) const
        {
            auto const numBones = m_boneIDs.size();
            for ( auto i = 0; i < numBones; i++ )
            {
                if ( m_boneIDs[i] == boneID )
                {
                    return i;
                }
            }

            return InvalidIndex;
        }
    }
}