#include "AnimationSkeletonLoader.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    SkeletonLoader::SkeletonLoader()
    {
        m_loadableTypes.push_back( Skeleton::GetStaticResourceTypeID() );
    }

    bool SkeletonLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        Skeleton* pSkeleton = KRG::New<Skeleton>();
        archive >> *pSkeleton;
        KRG_ASSERT( pSkeleton->IsValid() );
        pResourceRecord->SetResourceData( pSkeleton );

        // Calculate global reference pose
        //-------------------------------------------------------------------------

        int32 const numBones = pSkeleton->GetNumBones();
        pSkeleton->m_globalReferencePose.resize( numBones );

        pSkeleton->m_globalReferencePose[0] = pSkeleton->m_localReferencePose[0];
        for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
        {
            int32 const parentIdx = pSkeleton->GetParentIndex( boneIdx );
            pSkeleton->m_globalReferencePose[boneIdx] = pSkeleton->m_localReferencePose[boneIdx] * pSkeleton->m_globalReferencePose[parentIdx];
        }

        //-------------------------------------------------------------------------

        return true;
    }
}