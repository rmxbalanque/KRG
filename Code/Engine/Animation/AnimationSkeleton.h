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

            inline S32 GetNumBones() const { return (S32) m_boneIDs.size(); }
            inline bool IsValidBoneIndex( S32 idx ) const { return idx >= 0 && idx < m_boneIDs.size(); }
            inline S32 GetBoneIndex( StringID const& ID ) const { return VectorFindIndex( m_boneIDs, ID ); }
            inline StringID GetBoneID( S32 idx ) const;
            inline TVector<S32> const& GetParentIndices() const { return m_parentMap; }
            inline S32 GetParentIndex( S32 idx ) const;
            TVector<Transform> const& GetReferencePose() const { return m_referencePose; }

            inline Transform GetBoneTransform( S32 idx ) const;
            Transform GetBoneGlobalTransform( S32 idx ) const;
        private:

            TVector<StringID>           m_boneIDs;
            TVector<S32>                m_parentMap;
            TVector<Transform>          m_referencePose;
        };

        //-------------------------------------------------------------------------

        inline StringID Skeleton::GetBoneID( S32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_boneIDs.size() );
            return m_boneIDs[idx];
        }

        inline S32 Skeleton::GetParentIndex( S32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_parentMap.size() );
            return m_parentMap[idx]; 
        }

        inline Transform Skeleton::GetBoneTransform( S32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_referencePose.size() );
            return m_referencePose[idx];
        }
    }
}