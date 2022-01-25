#include "AnimationGraphLoader.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    GraphLoader::GraphLoader()
    {
        m_loadableTypes.push_back( GraphDataSet::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( GraphDefinition::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( GraphVariation::GetStaticResourceTypeID() );
    }

    bool GraphLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == GraphDefinition::GetStaticResourceTypeID() )
        {
            auto* pGraphDef = KRG::New<GraphDefinition>();
            archive >> *pGraphDef;
            pResourceRecord->SetResourceData( pGraphDef );

            // Deserialize debug node paths
            //-------------------------------------------------------------------------

            //#if !KRG_SHIPPING
            archive >> pGraphDef->m_nodePaths;
            //#endif

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
        else if ( resourceTypeID == GraphDataSet::GetStaticResourceTypeID() )
        {
            GraphDataSet* pDataSet = KRG::New<GraphDataSet>();
            archive >> *pDataSet;
            KRG_ASSERT( pDataSet->m_variationID.IsValid() && pDataSet->m_pSkeleton.IsValid() );
            pResourceRecord->SetResourceData( pDataSet );
            return true;
        }
        else if ( resourceTypeID == GraphVariation::GetStaticResourceTypeID() )
        {
            GraphVariation* pGraphVariation = KRG::New<GraphVariation>();
            archive >> *pGraphVariation;
            pResourceRecord->SetResourceData( pGraphVariation );
            return true;
        }

        KRG_UNREACHABLE_CODE();
        return false;
    }

    Resource::InstallResult GraphLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == GraphDataSet::GetStaticResourceTypeID() )
        {
            auto pDataSet = pResourceRecord->GetResourceData<GraphDataSet>();

            KRG_ASSERT( pDataSet->m_pSkeleton.GetResourceID().IsValid() );
            pDataSet->m_pSkeleton = GetInstallDependency( installDependencies, pDataSet->m_pSkeleton.GetResourceID() );

            if ( !pDataSet->IsValid() )
            {
                KRG_LOG_ERROR( "Animation", "Failed to install skeleton for graph data set resource: %s", resID.ToString().c_str() );
                return Resource::InstallResult::Failed;
            }

            //-------------------------------------------------------------------------

            int32 const numInstallDependencies = (int32) installDependencies.size();
            for ( auto i = 1; i < numInstallDependencies; i++ )
            {
                pDataSet->m_resources[i - 1] = GetInstallDependency( installDependencies, pDataSet->m_resources[i - 1].GetResourceID() );
            }
        }
        else if ( resourceTypeID == GraphVariation::GetStaticResourceTypeID() )
        {
            auto pGraphVariation = pResourceRecord->GetResourceData<GraphVariation>();
            pGraphVariation->m_pDataSet = GetInstallDependency( installDependencies, pGraphVariation->m_pDataSet.GetResourceID() );
            pGraphVariation->m_pGraphDefinition = GetInstallDependency( installDependencies, pGraphVariation->m_pGraphDefinition.GetResourceID() );
        }

        //-------------------------------------------------------------------------

        ResourceLoader::Install( resID, pResourceRecord, installDependencies );
        return Resource::InstallResult::Succeeded;
    }

    void GraphLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto const resourceTypeID = resID.GetResourceTypeID();

        if ( resourceTypeID == GraphDefinition::GetStaticResourceTypeID() )
        {
            auto pGraphDef = pResourceRecord->GetResourceData<GraphDefinition>();
            if ( pGraphDef != nullptr )
            {
                TypeSystem::TypeDescriptorCollection::DestroyStaticCollection( pGraphDef->m_nodeSettings );
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}