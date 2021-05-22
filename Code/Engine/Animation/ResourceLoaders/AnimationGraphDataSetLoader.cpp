#include "AnimationGraphDataSetLoader.h"
#include "Engine/Animation/Graph/AnimationGraphDataset.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "../../../System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphDataSetLoader::AnimationGraphDataSetLoader()
    {
        m_loadableTypes.push_back( AnimationGraphDataSet::GetStaticResourceTypeID() );
    }

    bool AnimationGraphDataSetLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        AnimationGraphDataSet* pDataSet = KRG::New<AnimationGraphDataSet>();
        archive >> *pDataSet;
        KRG_ASSERT( pDataSet->m_name.IsValid() && pDataSet->m_pSkeleton.IsValid() );
        pResourceRecord->SetResourceData( pDataSet );

        return true;
    }

    bool AnimationGraphDataSetLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        AnimationGraphDataSet* pDataSet = pResourceRecord->GetResourceData<AnimationGraphDataSet>();

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

        //-------------------------------------------------------------------------

        ResourceLoader::Install( resID, pResourceRecord, installDependencies );
        return true;
    }
}