#pragma once 

#include "_Module/API.h"
#include "AnimationSkeleton.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Debug { class DrawingContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API Pose
    {
        friend class Blender;

    public:

        enum class InitialState
        {
            None,
            ReferencePose,
            ZeroPose
        };

        enum class State
        {
            Unset,
            Pose,
            ReferencePose,
            ZeroPose,
            AdditivePose
        };

    public:

        Pose( Skeleton const* pSkeleton, InitialState intialState = InitialState::ReferencePose );

        // Move
        Pose( Pose&& rhs );
        Pose& operator=( Pose&& rhs );

        // Explicitly disable the copy operation to prevent accidental copies
        Pose( Pose const& rhs ) = delete;
        Pose& operator=( Pose const& rhs ) = delete;

        void CopyFrom( Pose const& rhs );
        KRG_FORCE_INLINE void CopyFrom( Pose const* pRhs ) { CopyFrom( *pRhs ); }

        //-------------------------------------------------------------------------

        inline int32 GetNumBones() const { return m_pSkeleton->GetNumBones(); }
        inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }

        // Pose state
        //-------------------------------------------------------------------------

        void Reset( InitialState initState = InitialState::None, bool calcGlobalPose = false );

        inline bool IsPoseSet() const { return m_state != State::Unset; }
        inline bool IsReferencePose() const { return m_state == State::ReferencePose; }
        inline bool IsZeroPose() const { return m_state == State::ZeroPose; }
        inline bool IsAdditivePose() const { return m_state == State::AdditivePose; }

        // Local Transforms
        //-------------------------------------------------------------------------

        TVector<Transform> const& GetTransforms() const { return m_localTransforms; }

        inline Transform const& GetTransform( int32 boneIdx ) const
        {
            KRG_ASSERT( boneIdx < GetNumBones() );
            return m_localTransforms[boneIdx];
        }

        inline void SetTransform( int32 boneIdx, Transform const& transform )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx] = transform;
        }

        inline void SetRotation( int32 boneIdx, Quaternion const& rotation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetRotation( rotation );
        }

        inline void SetTranslation( int32 boneIdx, Float3 const& translation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetTranslation( translation );
        }

        inline void SetScale( int32 boneIdx, Float3 const& scale )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetScale( scale );
        }

        // Global Transform Cache
        //-------------------------------------------------------------------------

        inline bool HasGlobalTransforms() const { return !m_globalTransforms.empty(); }
        inline void ClearGlobalTransforms() { m_globalTransforms.clear(); }
        inline TVector<Transform> const& GetGlobalTransforms() const { return m_globalTransforms; }
        void CalculateGlobalTransforms();
        Transform GetGlobalTransform( int32 boneIdx ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void DrawDebug( Debug::DrawingContext& ctx, Transform const& worldTransform ) const;
        #endif

    private:

        Pose() = delete;

        void SetToReferencePose( bool setGlobalPose );
        void SetToZeroPose( bool setGlobalPose );

    private:

        Skeleton const*             m_pSkeleton;            // The skeleton for this pose
        TVector<Transform>          m_localTransforms;      // Parent-space transforms
        TVector<Transform>          m_globalTransforms;     // Character-space transforms
        State                       m_state = State::Unset; // Pose state
    };
}