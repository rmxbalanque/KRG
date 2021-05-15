#pragma once
#include "System/Resource/ResourcePtr.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphDataSet
    {
    public:

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        inline AnimationClip* GetAnimationClip( UUID const& ID ) const
        {
            auto iter = m_pAnimations.find( ID );
            if ( iter != m_pAnimations.end() )
            {
                return iter->second;
            }

            return nullptr;
        }

    private:

        TResourcePtr<Skeleton>              m_pSkeleton = nullptr;
        THashMap<UUID, AnimationClip*>      m_pAnimations;
    };
}