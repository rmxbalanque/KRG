#pragma once
#include "AnimationGraphNode.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    namespace Graph { class GraphInstance; }
    using namespace Graph;

    //-------------------------------------------------------------------------

    using DataSetSlotIndex = int16;

    class KRG_ENGINE_ANIMATION_API AnimationGraphDataSet : public Resource::IResource
    {
        KRG_REGISTER_VIRTUAL_RESOURCE( 'AGDS', "Animation Graph DataSet" );
        KRG_SERIALIZE_MEMBERS( m_variationID, m_pSkeleton, m_resources );
        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class GraphInstance;

    public:

        virtual bool IsValid() const override { return m_variationID.IsValid() && m_pSkeleton.IsLoaded(); }

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        inline AnimationClip const* GetAnimationClip( DataSetSlotIndex const& ID ) const
        {
            KRG_ASSERT( ID >= 0 && ID < m_resources.size() );
            TResourcePtr<AnimationClip> pAnimationClip( m_resources[ID] );
            return pAnimationClip.GetPtr();
        }

    private:

        StringID                                    m_variationID;
        TResourcePtr<Skeleton>                      m_pSkeleton = nullptr;
        TVector<Resource::ResourcePtr>              m_resources;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationGraphDefinition : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'AG', "Animation Graph" );
        KRG_SERIALIZE_MEMBERS( m_persistentNodeIndices, m_instanceNodeStartOffsets, m_instanceRequiredMemory, m_instanceRequiredAlignment, m_numControlParameters, m_rootNodeIdx );

        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class GraphInstance;

    public:

        virtual bool IsValid() const override { return m_rootNodeIdx != InvalidIndex; }

    protected:

        TVector<NodeIndex>                          m_persistentNodeIndices;
        TVector<uint32>                             m_instanceNodeStartOffsets;
        uint32                                      m_instanceRequiredMemory = 0;
        uint32                                      m_instanceRequiredAlignment = 0;
        int32                                       m_numControlParameters = 0;
        NodeIndex                                   m_rootNodeIdx = InvalidIndex;

        TVector<GraphNode::Settings*>               m_nodeSettings;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationGraphVariation : public Resource::IResource
    {
        KRG_REGISTER_VIRTUAL_RESOURCE( 'AGV', "Animation Graph Variation" );
        KRG_SERIALIZE_MEMBERS( m_pGraphDefinition, m_pDataSet );

        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class GraphInstance;

    public:

        static StringID const DefaultVariationID;

    public:

        virtual bool IsValid() const override
        {
            return m_pGraphDefinition.IsLoaded() && m_pDataSet.IsLoaded();
        }

        inline Skeleton const* GetSkeleton() const 
        {
            KRG_ASSERT( IsValid() );
            return m_pDataSet->GetSkeleton();
        }

    protected:

        TResourcePtr<AnimationGraphDefinition>   m_pGraphDefinition = nullptr;
        TResourcePtr<AnimationGraphDataSet>      m_pDataSet = nullptr;
    };
}