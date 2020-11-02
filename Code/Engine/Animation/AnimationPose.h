#pragma once 

#include "_Module/API.h"
#include "AnimationSkeleton.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_ENGINE_ANIMATION_API Pose
        {

        public:

            enum class Init
            {
                None,
                ReferencePose
            };

        public:

            Pose( Skeleton const* pSkeleton, Init opt = Init::ReferencePose );
            Pose( Pose const& rhs ) { CopyFrom( &rhs ); }
            void CopyFrom( Pose const* pPose );

            void SetReferencePose();
            void SetZeroPose();

            inline bool IsAdditive() const { return m_isAdditive; }
            inline void SetAdditiveFlag( bool isAdditive ) { m_isAdditive = isAdditive; }

            inline S32 GetNumBones() const { return m_pSkeleton->GetNumBones(); }
            inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }

            TVector<Transform> const& GetTransforms() const { return m_localTransforms; }
            inline Transform const& GetTransform( S32 boneIdx ) const;

            inline void SetTransform( S32 boneIdx, Transform const& transform );
            inline void SetRotation( S32 boneIdx, Quaternion const& rotation );
            inline void SetTranslation( S32 boneIdx, Float3 const& translation );
            inline void SetScale( S32 boneIdx, Float3 const& scale );

            inline bool HasGlobalTransforms() const { return !m_globalTransforms.empty(); }
            inline void ClearGlobalTransforms() { m_globalTransforms.clear(); }
            inline TVector<Transform> const& GetGlobalTransforms() const { return m_globalTransforms; }
            void CalculateGlobalTransforms();
            Transform GetGlobalTransform( S32 boneIdx ) const;

        private:

            Pose() = delete;
            Pose& operator=( Pose const& rhs ) = delete;

        private:

            Skeleton const*             m_pSkeleton;            // The skeleton for this pose
            TVector<Transform>          m_localTransforms;      // Parent-space transforms
            TVector<Transform>          m_globalTransforms;     // Character-space transforms
            bool                        m_isAdditive;
        };

        //-------------------------------------------------------------------------

        inline Transform const& Pose::GetTransform( S32 boneIdx ) const
        {
            KRG_ASSERT( boneIdx < GetNumBones() );
            return m_localTransforms[boneIdx];
        }

        inline void Pose::SetTransform( S32 boneIdx, Transform const& transform )
        { 
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 ); 
            m_localTransforms[boneIdx] = transform;
        }

        inline void Pose::SetRotation( S32 boneIdx, Quaternion const& rotation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetRotation( rotation );
        }

        inline void Pose::SetTranslation( S32 boneIdx, Float3 const& translation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetTranslation( translation );
        }

        inline void Pose::SetScale( S32 boneIdx, Float3 const& scale )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetScale( scale );
        }
    }
}