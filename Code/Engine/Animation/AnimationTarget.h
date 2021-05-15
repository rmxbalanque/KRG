#pragma once

#include "System/Core/Math/Transform.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class Pose;

    //-------------------------------------------------------------------------

    class Target
    {
    public:

        Target() = default;

        explicit Target( Transform const& T )
            : m_transform( T.GetRotation(), T.GetTranslation() ) // Ignore scale for targets
            , m_isSet( true )
        {}

        explicit Target( Quaternion const& R, Vector const& T )
            : m_transform( R, T )
            , m_isSet( true )
        {}

        explicit Target( StringID boneID )
            : m_boneID( boneID )
            , m_isSet( true )
        {}

        inline void Reset() { m_isSet = false; }
        inline bool IsTargetSet() const { return m_isSet; }

        inline bool IsBoneTarget() const { return m_boneID.IsValid(); }
        inline StringID GetBoneID() const { KRG_ASSERT( IsBoneTarget() ); return m_boneID; }

        // Offsets
        //-------------------------------------------------------------------------

        inline bool HasOffsets() const { return m_hasOffsets; }
        inline Quaternion GetRotationOffset() const { return m_rotationOffset; }
        inline Vector GetTranslationOffset() const { return m_translationOffset; }
        inline bool IsUsingBoneSpaceOffsets() const { return m_isBoneSpaceOffsets; }

        inline void SetOffsets( Quaternion const& rotationOffset, Vector const& translationOffset, bool useBoneSpaceOffsets = true )
        {
            KRG_ASSERT( m_isSet );
            m_rotationOffset = rotationOffset;
            m_translationOffset = translationOffset;
            m_isBoneSpaceOffsets = IsBoneTarget() ? useBoneSpaceOffsets : false; // Offset space only makes sense for bone targets
            m_hasOffsets = true;
        }

        // Transform
        //-------------------------------------------------------------------------

        inline Transform GetTransform() const
        {
            KRG_ASSERT( m_isSet && !IsBoneTarget() );

            Transform targetTransform = m_transform;
            if ( m_hasOffsets )
            {
                targetTransform.SetRotation( targetTransform.GetRotation() * m_rotationOffset );
                targetTransform.SetTranslation( targetTransform.GetTranslation() + m_translationOffset );
            }

            return targetTransform;
        }

        bool TryGetTransform( Pose const* pPose, Transform& outTransform ) const;

    private:

        Transform           m_transform = Transform::Identity;
        Quaternion          m_rotationOffset = Quaternion::Identity;
        Vector              m_translationOffset = Vector::Zero;
        StringID            m_boneID;
        bool                m_isBoneSpaceOffsets = true;
        bool                m_hasOffsets = false;
        bool                m_isSet = false;
    };
}