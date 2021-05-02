#include "AnimationLoader.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/TypeSystem/Serialization/ImmutableTypeCollection.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationLoader::AnimationLoader()
    {
        m_loadableTypes.push_back( AnimationClip::GetStaticResourceTypeID() );
    }

    void AnimationLoader::SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry )
    {
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_pTypeRegistry = pTypeRegistry;
    }

    bool AnimationLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
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

        TypeSystem::ImmutableTypeCollectionHeader collectionHeader;
        archive >> collectionHeader;

        if ( collectionHeader.m_totalRequiredSize > 0 )
        {
            TVector<TypeSystem::ImmutableTypeDescriptor> eventTypeDescriptors;
            archive >> eventTypeDescriptors;

            pAnimation->m_pEventsRawMemory = KRG::Alloc( collectionHeader.m_totalRequiredSize, collectionHeader.m_requiredAlignment );
            TypeSystem::CreateImmutableTypeCollection( *m_pTypeRegistry, (Byte*) pAnimation->m_pEventsRawMemory, eventTypeDescriptors, pAnimation->m_events );
        }

        return true;
    }

    void AnimationLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pAnimation = pResourceRecord->GetResourceData<AnimationClip>();
        if ( pAnimation != nullptr )
        {
            // Release allocated events collection
            if ( pAnimation->m_pEventsRawMemory != nullptr )
            {
                TypeSystem::DestroyImmutableTypeCollection( pAnimation->m_events );
                KRG::Free( pAnimation->m_pEventsRawMemory );
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
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