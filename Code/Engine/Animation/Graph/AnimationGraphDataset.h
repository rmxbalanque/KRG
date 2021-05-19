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
        KRG_SERIALIZE_MEMBERS( m_name, m_pSkeleton, m_dataRecords );
        friend class AnimationGraphDataSetCompiler;
        friend class AnimationGraphDataSetLoader;
        friend class AnimationGraphDataSetInstance;

        struct DataRecord
        {
            KRG_SERIALIZE_MEMBERS( m_ID, m_pResource );

            DataRecord() = default;

            DataRecord( UUID const& ID, ResourceID const& animationClipID )
                : m_ID( ID )
                , m_pResource( animationClipID )
            {
                KRG_ASSERT( ID.IsValid() );
            }

            UUID                                    m_ID;
            TResourcePtr<AnimationClip>             m_pResource;
        };

    public:

        virtual bool IsValid() const override { return m_name.IsValid() && m_pSkeleton.IsLoaded(); }

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        inline AnimationClip const* GetAnimationClip( UUID const& ID ) const
        {
            auto iter = m_animationLookupMap.find( ID );
            if ( iter != m_animationLookupMap.end() )
            {
                return iter->second;
            }

            return nullptr;
        }

    private:

        StringID                                    m_name;
        TResourcePtr<Skeleton>                      m_pSkeleton = nullptr;
        TVector<DataRecord>                         m_dataRecords;
        THashMap<UUID, AnimationClip const*>        m_animationLookupMap;
    };
}