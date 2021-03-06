#ifdef _WIN32
#include "RawSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        RawSkeleton::BoneData::BoneData( char const* pName )
            : m_name( pName )
        {
            KRG_ASSERT( m_name.IsValid() );
        }

        //-------------------------------------------------------------------------

        int32 RawSkeleton::GetBoneIndex( StringID const& boneName ) const
        {
            int32 const numBones = GetNumBones();

            for ( auto i = 0; i < numBones; i++ )
            {
                if ( m_bones[i].m_name == boneName )
                {
                    return i;
                }
            }

            return InvalidIndex;
        }  

        void RawSkeleton::CalculateLocalTransforms()
        {
            KRG_ASSERT( !m_bones.empty() );
            m_bones[0].m_localTransform = m_bones[0].m_globalTransform;

            auto const numBones = m_bones.size();
            for ( auto i = numBones - 1; i > 0; i-- )
            {
                int32 parentIdx = m_bones[i].m_parentBoneIdx;
                auto const& parentInverseTransform = m_bones[parentIdx].m_globalTransform.GetInverse();
                m_bones[i].m_localTransform = m_bones[i].m_globalTransform * parentInverseTransform;
            }
        }

        void RawSkeleton::CalculateGlobalTransforms()
        {
            KRG_ASSERT( !m_bones.empty() );
            m_bones[0].m_globalTransform = m_bones[0].m_localTransform;

            auto const numBones = m_bones.size();
            for ( auto i = 1; i < numBones; i++ )
            {
                int32 parentIdx = m_bones[i].m_parentBoneIdx;
                KRG_ASSERT( parentIdx != InvalidIndex );
                m_bones[i].m_globalTransform = m_bones[i].m_localTransform * m_bones[parentIdx].m_globalTransform;
            }
        }
    }
}
#endif