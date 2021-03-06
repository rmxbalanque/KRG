#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_ENGINE_ANIMATION_API Skeleton : public Resource::IResource
        {
            KRG_REGISTER_RESOURCE( 'SKEL' );
            KRG_SERIALIZE_MEMBERS( m_boneIDs, m_referencePose, m_parentMap );
            friend class SkeletonCompiler;

        public:

            virtual bool IsValid() const final;

            inline int32 GetNumBones() const { return (int32) m_boneIDs.size(); }
            inline bool IsValidBoneIndex( int32 idx ) const { return idx >= 0 && idx < m_boneIDs.size(); }
            inline int32 GetBoneIndex( StringID const& ID ) const { return VectorFindIndex( m_boneIDs, ID ); }
            inline StringID GetBoneID( int32 idx ) const;
            inline TVector<int32> const& GetParentIndices() const { return m_parentMap; }
            inline int32 GetParentIndex( int32 idx ) const;
            TVector<Transform> const& GetReferencePose() const { return m_referencePose; }

            inline Transform GetBoneTransform( int32 idx ) const;
            Transform GetBoneGlobalTransform( int32 idx ) const;
        private:

            TVector<StringID>           m_boneIDs;
            TVector<int32>                m_parentMap;
            TVector<Transform>          m_referencePose;
        };

        //-------------------------------------------------------------------------

        inline StringID Skeleton::GetBoneID( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_boneIDs.size() );
            return m_boneIDs[idx];
        }

        inline int32 Skeleton::GetParentIndex( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_parentMap.size() );
            return m_parentMap[idx]; 
        }

        inline Transform Skeleton::GetBoneTransform( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_referencePose.size() );
            return m_referencePose[idx];
        }
    }
}