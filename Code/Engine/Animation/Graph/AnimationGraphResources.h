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

    using DataSetSourceIndex = int16;

    class AnimationGraphDataSet : public Resource::IResource
    {
        KRG_REGISTER_VIRTUAL_RESOURCE( 'AGDS' );
        KRG_SERIALIZE_MEMBERS( m_name, m_pSkeleton, m_animationClips );
        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class GraphInstance;

    public:

        virtual bool IsValid() const override { return m_name.IsValid() && m_pSkeleton.IsLoaded(); }

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        inline AnimationClip const* GetAnimationClip( DataSetSourceIndex const& ID ) const
        {
            KRG_ASSERT( ID >= 0 && ID < m_animationClips.size() );
            return m_animationClips[ID].GetPtr();
        }

    private:

        StringID                                    m_name;
        TResourcePtr<Skeleton>                      m_pSkeleton = nullptr;
        TVector<TResourcePtr<AnimationClip>>        m_animationClips;
    };

    //-------------------------------------------------------------------------

    class AnimationGraphDefinition : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'AG' );
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

    class AnimationGraphVariation : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'AGV' );
        KRG_SERIALIZE_MEMBERS( m_pGraphDefinition, m_pDataSet );

        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class GraphInstance;

    public:

        constexpr static char const* const s_pDefaultVariationName = "Default";

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