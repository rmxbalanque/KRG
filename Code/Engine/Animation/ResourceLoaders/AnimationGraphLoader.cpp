#include "AnimationGraphLoader.h"
#include "Engine/Animation/Graph/AnimationGraph.h"
#include "System/Core/Serialization/BinaryArchive.h"

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

        AnimationGraph* pDataSet = KRG::New<AnimationGraph>();
        archive >> *pDataSet;
        pResourceRecord->SetResourceData( pDataSet );

        return true;
    }
}