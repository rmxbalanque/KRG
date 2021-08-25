#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/BitFlags.h"

//-------------------------------------------------------------------------

namespace KRG::Debug { class DrawingContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    // Per-bone flags to provide extra information
    enum class BoneFlags
    {
        None,
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API Skeleton : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'SKEL' );
        KRG_SERIALIZE_MEMBERS( m_boneIDs, m_localReferencePose, m_parentIndices, m_boneFlags );

        friend class SkeletonCompiler;
        friend class SkeletonLoader;

    public:

        virtual bool IsValid() const final;
        inline int32 GetNumBones() const { return (int32) m_boneIDs.size(); }

        // Bone info
        //-------------------------------------------------------------------------

        inline bool IsValidBoneIndex( int32 idx ) const { return idx >= 0 && idx < m_boneIDs.size(); }

        inline int32 GetBoneIndex( StringID const& ID ) const { return VectorFindIndex( m_boneIDs, ID ); }

        inline StringID GetBoneID( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_boneIDs.size() );
            return m_boneIDs[idx];
        }

        inline TVector<int32> const& GetParentBoneIndices() const { return m_parentIndices; }

        inline int32 GetParentBoneIndex( int32 idx ) const
        {
            KRG_ASSERT( idx >= 0 && idx < m_parentIndices.size() );
            return m_parentIndices[idx];
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
        void DrawDebug( Debug::DrawingContext& ctx, Transform const& worldTransform ) const;
        #endif

    private:

        TVector<StringID>                   m_boneIDs;
        TVector<int32>                      m_parentIndices;
        TVector<Transform>                  m_localReferencePose;
        TVector<Transform>                  m_globalReferencePose;
        TVector<TBitFlags<BoneFlags>>       m_boneFlags;
    };
}