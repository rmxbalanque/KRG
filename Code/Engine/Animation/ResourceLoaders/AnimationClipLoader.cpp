#include "AnimationClipLoader.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationClipLoader::AnimationClipLoader()
    {
        m_loadableTypes.push_back( AnimationClip::GetStaticResourceTypeID() );
    }

    void AnimationClipLoader::SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry )
    {
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_pTypeRegistry = pTypeRegistry;
    }

    bool AnimationClipLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() && m_pTypeRegistry != nullptr );

        auto pAnimation = KRG::New<AnimationClip>();
        archive >> *pAnimation;
        pResourceRecord->SetResourceData( pAnimation );

        // Read sync events
        //-------------------------------------------------------------------------

        TInlineVector<SyncTrack::EventMarker, 10> syncEventMarkers;
        archive >> syncEventMarkers;
        pAnimation->m_syncTrack = SyncTrack( syncEventMarkers, 0 );

        // Read events
        //-------------------------------------------------------------------------

        TypeSystem::TypeDescriptorCollection collectionDesc;
        archive >> collectionDesc;

        collectionDesc.CalculateCollectionRequirements( *m_pTypeRegistry );
        TypeSystem::TypeDescriptorCollection::InstantiateStaticCollection( *m_pTypeRegistry, collectionDesc, pAnimation->m_events );

        return true;
    }

    void AnimationClipLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pAnimation = pResourceRecord->GetResourceData<AnimationClip>();
        if ( pAnimation != nullptr )
        {
            // Release allocated events collection
            if ( !pAnimation->m_events.empty() )
            {
                TypeSystem::TypeDescriptorCollection::DestroyStaticCollection( pAnimation->m_events );
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }

    Resource::InstallResult AnimationClipLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        auto pAnimData = pResourceRecord->GetResourceData<AnimationClip>();
        KRG_ASSERT( pAnimData->m_pSkeleton.GetResourceID().IsValid() );

        pAnimData->m_pSkeleton = GetInstallDependency( installDependencies, pAnimData->m_pSkeleton.GetResourceID() );
        KRG_ASSERT( pAnimData->IsValid() );

        ResourceLoader::Install( resID, pResourceRecord, installDependencies );

        return Resource::InstallResult::Succeeded;
    }
}