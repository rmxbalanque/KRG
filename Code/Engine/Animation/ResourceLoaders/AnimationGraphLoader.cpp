#include "AnimationGraphLoader.h"
#include "Engine/Animation/Graph/AnimationGraph.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/TypeSystem/TypeDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphLoader::AnimationGraphLoader()
    {
        m_loadableTypes.push_back( AnimationGraph::GetStaticResourceTypeID() );
    }

    bool AnimationGraphLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        AnimationGraph* pGraph = KRG::New<AnimationGraph>();
        archive >> *pGraph;
        pResourceRecord->SetResourceData( pGraph );

        // Create Settings
        //-------------------------------------------------------------------------

        TypeSystem::TypeDescriptorCollection typeDescriptors;
        archive >> typeDescriptors;

        typeDescriptors.CalculateCollectionRequirements( *m_pTypeRegistry );
        TypeSystem::TypeDescriptorCollection::InstantiateStaticCollection( *m_pTypeRegistry, typeDescriptors, pGraph->m_nodeSettings );

        cereal::BinaryInputArchive& settingsArchive = *archive.GetInputArchive();
        for ( auto pSettings : pGraph->m_nodeSettings )
        {
            pSettings->Load( settingsArchive );
        }

        //-------------------------------------------------------------------------

        return pGraph->IsValid();
    }

    void AnimationGraphLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pGraph = pResourceRecord->GetResourceData<AnimationGraph>();
        if ( pGraph != nullptr )
        {
            TypeSystem::TypeDescriptorCollection::DestroyStaticCollection( pGraph->m_nodeSettings );
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}