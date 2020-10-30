#include "AnimationSkeletonLoader.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Animation
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

            return true;
        }
    }
}