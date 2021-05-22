#pragma once
#include "System/Resource/ResourcePtr.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphDataSet : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'AGDS' );
        KRG_SERIALIZE_MEMBERS( m_name, m_pSkeleton, m_animationClips );
        friend class AnimationGraphDataSetCompiler;
        friend class AnimationGraphDataSetLoader;
        friend class AnimationGraphDataSetInstance;

    public:

        virtual bool IsValid() const override { return m_name.IsValid() && m_pSkeleton.IsLoaded(); }

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        inline AnimationClip const* GetAnimationClip( uint16 const& ID ) const
        {
            KRG_ASSERT( ID < m_animationClips.size() );
            return m_animationClips[ID].GetPtr();
        }

    private:

        StringID                                    m_name;
        TResourcePtr<Skeleton>                      m_pSkeleton = nullptr;
        TVector<TResourcePtr<AnimationClip>>        m_animationClips;
    };
}