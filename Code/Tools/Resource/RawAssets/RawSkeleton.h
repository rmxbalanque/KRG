#pragma once

#include "RawAsset.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    class KRG_TOOLS_RESOURCE_API RawSkeleton : public RawAsset
    {

    public:

        struct BoneData
        {
            BoneData( const char* pName );

        public:

            StringID            m_name;
            Transform           m_localTransform = Transform::Identity;
            Transform           m_globalTransform = Transform::Identity;
            int32               m_parentBoneIdx = InvalidIndex;
        };

    public:

        RawSkeleton() = default;
        virtual ~RawSkeleton() {}

        virtual bool IsValid() const override final { return !m_bones.empty(); }

        inline StringID const& GetName() const { return m_name; }
        inline BoneData const& GetRootBone() const { return m_bones[0]; }
        inline TVector<BoneData> const& GetBoneData() const { return m_bones; }
        inline BoneData const& GetBoneData( int32 boneIdx ) const { KRG_ASSERT( boneIdx >= 0 && boneIdx < m_bones.size() ); return m_bones[boneIdx]; }
        inline StringID const& GetBoneName( int32 boneIdx ) const { KRG_ASSERT( boneIdx >= 0 && boneIdx < m_bones.size() ); return m_bones[boneIdx].m_name; }
        inline int32 GetNumBones() const { return (int32) m_bones.size(); }

        int32 GetBoneIndex( StringID const& boneName ) const;
        inline int32 GetParentIndex( int32 boneIdx ) const { KRG_ASSERT( boneIdx >= 0 && boneIdx < m_bones.size() ); return m_bones[boneIdx].m_parentBoneIdx; }
        inline Transform const& GetLocalTransform( int32 boneIdx ) const { KRG_ASSERT( boneIdx >= 0 && boneIdx < m_bones.size() ); return m_bones[boneIdx].m_localTransform; }
        inline Transform const& GetGlobalTransform( int32 boneIdx ) const { KRG_ASSERT( boneIdx >= 0 && boneIdx < m_bones.size() ); return m_bones[boneIdx].m_globalTransform; }

    protected:

        void CalculateLocalTransforms();
        void CalculateGlobalTransforms();

    protected:

        StringID                m_name;
        TVector<BoneData>       m_bones;
    };
}