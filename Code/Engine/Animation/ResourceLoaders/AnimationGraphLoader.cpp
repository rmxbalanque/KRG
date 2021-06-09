#include "AnimationGraphLoader.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphLoader::AnimationGraphLoader()
    {
        m_loadableTypes.push_back( AnimationGraphDataSet::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( AnimationGraphDefinition::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( AnimationGraphVariation::GetStaticResourceTypeID() );
    }

    bool AnimationGraphLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == AnimationGraphDefinition::GetStaticResourceTypeID() )
        {
            auto* pGraphDef = KRG::New<AnimationGraphDefinition>();
            archive >> *pGraphDef;
            pResourceRecord->SetResourceData( pGraphDef );

            // Create Settings
            //-------------------------------------------------------------------------

            TypeSystem::TypeDescriptorCollection typeDescriptors;
            archive >> typeDescriptors;

            typeDescriptors.CalculateCollectionRequirements( *m_pTypeRegistry );
            TypeSystem::TypeDescriptorCollection::InstantiateStaticCollection( *m_pTypeRegistry, typeDescriptors, pGraphDef->m_nodeSettings );

            cereal::BinaryInputArchive& settingsArchive = *archive.GetInputArchive();
            for ( auto pSettings : pGraphDef->m_nodeSettings )
            {
                pSettings->Load( settingsArchive );
            }

            //-------------------------------------------------------------------------

            return pGraphDef->IsValid();
        }
        else if ( resourceTypeID == AnimationGraphDataSet::GetStaticResourceTypeID() )
        {
            AnimationGraphDataSet* pDataSet = KRG::New<AnimationGraphDataSet>();
            archive >> *pDataSet;
            KRG_ASSERT( pDataSet->m_name.IsValid() && pDataSet->m_pSkeleton.IsValid() );
            pResourceRecord->SetResourceData( pDataSet );
            return true;
        }
        else if ( resourceTypeID == AnimationGraphVariation::GetStaticResourceTypeID() )
        {
            AnimationGraphVariation* pGraphVariation = KRG::New<AnimationGraphVariation>();
            archive >> *pGraphVariation;
        }

        KRG_UNREACHABLE_CODE();
        return false;
    }

    bool AnimationGraphLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == AnimationGraphDataSet::GetStaticResourceTypeID() )
        {
            auto pDataSet = pResourceRecord->GetResourceData<AnimationGraphDataSet>();

            KRG_ASSERT( pDataSet->m_pSkeleton.GetResourceID().IsValid() );
            pDataSet->m_pSkeleton = GetInstallDependency( installDependencies, pDataSet->m_pSkeleton.GetResourceID() );

            if ( !pDataSet->IsValid() )
            {
                KRG_LOG_ERROR( "Animation", "Failed to install skeleton for graph data set resource: %s", resID.ToString().c_str() );
                return false;
            }

            //-------------------------------------------------------------------------

            int32 const numInstallDependencies = (int32) installDependencies.size();
            if ( pDataSet->m_animationClips.size() != numInstallDependencies - 1 )
            {
                KRG_LOG_ERROR( "Animation", "Invalid number of install dependencies for graph data set: %s", resID.ToString().c_str() );
                return false;
            }

            for ( auto i = 1; i < numInstallDependencies; i++ )
            {
                pDataSet->m_animationClips[i - 1] = installDependencies[i];
            }
        }
        else if ( resourceTypeID == AnimationGraphVariation::GetStaticResourceTypeID() )
        {
            auto pGraphVariation = pResourceRecord->GetResourceData<AnimationGraphVariation>();
            pGraphVariation->m_pDataSet = GetInstallDependency( installDependencies, pGraphVariation->m_pDataSet.GetResourceID() );
            pGraphVariation->m_pGraphDefinition = GetInstallDependency( installDependencies, pGraphVariation->m_pGraphDefinition.GetResourceID() );
        }

        //-------------------------------------------------------------------------

        ResourceLoader::Install( resID, pResourceRecord, installDependencies );
        return true;
    }

    void AnimationGraphLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == AnimationGraphDefinition::GetStaticResourceTypeID() )
        {
            auto pGraphDef = pResourceRecord->GetResourceData<AnimationGraphDefinition>();
            if ( pGraphDef != nullptr )
            {
                TypeSystem::TypeDescriptorCollection::DestroyStaticCollection( pGraphDef->m_nodeSettings );
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}