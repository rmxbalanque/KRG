#include "Component_AnimatedMesh.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimatedMeshComponent::SetSkeleton( ResourceID skeletonResourceID )
    {
        KRG_ASSERT( IsUnloaded() );
        KRG_ASSERT( skeletonResourceID.IsValid() );
        m_pSkeleton = skeletonResourceID;
    }

    void AnimatedMeshComponent::SetPose( Pose const* pPose )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        KRG_ASSERT( HasMeshResourceSet() );
        KRG_ASSERT( !m_animToMeshBoneMap.empty() );
        KRG_ASSERT( pPose != nullptr && pPose->HasGlobalTransforms() );

        int32 const numAnimBones = pPose->GetNumBones();
        for ( auto animBoneIdx = 0; animBoneIdx < numAnimBones; animBoneIdx++ )
        {
            int32 const meshBoneIdx = m_animToMeshBoneMap[animBoneIdx];
            if ( meshBoneIdx != InvalidIndex )
            {
                Transform const boneTransform = pPose->GetGlobalTransform( animBoneIdx );
                m_boneTransforms[meshBoneIdx] = boneTransform;
            }
        }
    }

    void AnimatedMeshComponent::Initialize()
    {
        SkeletalMeshComponent::Initialize();
        if ( HasMeshResourceSet() )
        {
            GenerateBoneMap();
        }
    }

    void AnimatedMeshComponent::Shutdown()
    {
        m_animToMeshBoneMap.clear();
        SkeletalMeshComponent::Shutdown();
    }

    void AnimatedMeshComponent::GenerateBoneMap()
    {
        KRG_ASSERT( m_pMesh != nullptr && m_pSkeleton != nullptr );

        auto const pMesh = GetMesh();

        auto const numBones = m_pSkeleton->GetNumBones();
        m_animToMeshBoneMap.resize( numBones, InvalidIndex );

        for ( auto boneIdx = 0; boneIdx < numBones; boneIdx++ )
        {
            auto const& boneID = m_pSkeleton->GetBoneID( boneIdx );
            m_animToMeshBoneMap[boneIdx] = pMesh->GetBoneIndex( boneID );
        }
    }
}