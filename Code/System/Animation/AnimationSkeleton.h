#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/BitFlags.h"

//-------------------------------------------------------------------------

namespace KRG::Drawing { class DrawContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    // Per-bone flags to provide extra information
    enum class BoneFlags
    {
        None,
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ANIMATION_API Skeleton : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'SKEL', "Animation Skeleton" );
        KRG_SERIALIZE_MEMBERS( m_boneIDs, m_localReferencePose, m_parentIndices, m_boneFlags );

        friend class SkeletonCompiler;
        friend class SkeletonLoader;

    public:

        virtual bool IsValid() const final;
        inline int32 GetNumBones() const { return (int32) m_boneIDs.size(); }

        // Bone info
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool IsValidBoneIndex( int32 idx ) const { return idx >= 0 && idx < m_boneIDs.size(); }

        // Get the index for a given bone ID, can return InvalidIndex
        inline int32 GetBoneIndex( StringID const& ID ) const { return VectorFindIndex( m_boneIDs, ID ); }

        // Get all parent indices
        inline TVector<int32> const& GetParentBoneIndices() const { return m_parentIndices; }

        // Get the direct parent for a given bone
        inline int32 GetParentBoneIndex( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_parentIndices.size() );
            return m_parentIndices[idx];
        }

        // Find the index of the first child encountered for the specified bone. Returns InvalidIndex if this is a leaf bone.
        int32 GetFirstChildBoneIndex( int32 boneIdx ) const;

        // Returns whether the specified bone is a child of the specified parent bone
        bool IsChildBoneOf( int32 parentBoneIdx, int32 childBoneIdx ) const;

        // Returns whether the specified bone is a parent of the specified child bone
        KRG_FORCE_INLINE bool IsParentBoneOf( int32 parentBoneIdx, int32 childBoneIdx ) const { return IsChildBoneOf( parentBoneIdx, childBoneIdx ); }

        // Returns whether the specified bone is a child of the specified parent bone
        KRG_FORCE_INLINE bool AreBonesInTheSameHierarchy( int32 boneIdx0, int32 boneIdx1 ) const { return IsChildBoneOf( boneIdx0, boneIdx1) || IsChildBoneOf( boneIdx1, boneIdx0 ); }

        // Get the boneID for a specified bone index
        KRG_FORCE_INLINE StringID GetBoneID( int32 idx ) const
        {
            KRG_ASSERT( IsValidBoneIndex( idx ) );
            return m_boneIDs[idx];
        }

        // Pose info
        //-------------------------------------------------------------------------

        TVector<Transform> const& GetLocalReferencePose() const { return m_localReferencePose; }
        TVector<Transform> const& GetGlobalReferencePose() const { return m_globalReferencePose; }

        inline Transform GetBoneTransform( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_localReferencePose.size() );
            return m_localReferencePose[idx];
        }

        Transform GetBoneGlobalTransform( int32 idx ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void DrawDebug( Drawing::DrawContext& ctx, Transform const& worldTransform ) const;
        #endif

    private:

        TVector<StringID>                   m_boneIDs;
        TVector<int32>                      m_parentIndices;
        TVector<Transform>                  m_localReferencePose;
        TVector<Transform>                  m_globalReferencePose;
        TVector<TBitFlags<BoneFlags>>       m_boneFlags;
    };

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    KRG_SYSTEM_ANIMATION_API void DrawRootBone( Drawing::DrawContext& ctx, Transform const& transform );
    #endif
}