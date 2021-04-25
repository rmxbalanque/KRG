#include "AnimationLoader.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        AnimationLoader::AnimationLoader()
        {
            m_loadableTypes.push_back( AnimationClip::GetStaticResourceTypeID() );
        }

        bool AnimationLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
        {
            KRG_ASSERT( archive.IsValid() );

            AnimationClip* pAnimData = KRG::New<AnimationClip>();
            archive >> *pAnimData;
            pResourceRecord->SetResourceData( pAnimData );

            return true;
        }

        bool AnimationLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
        {
            auto pAnimData = pResourceRecord->GetResourceData<AnimationClip>();
            KRG_ASSERT( pAnimData->m_pSkeleton.GetResourceID().IsValid() );

            pAnimData->m_pSkeleton = GetInstallDependency( installDependencies, pAnimData->m_pSkeleton.GetResourceID() );
            KRG_ASSERT( pAnimData->IsValid() );

            ResourceLoader::Install( resID, pResourceRecord, installDependencies );

            return true;
        }
    }
}