#pragma once

#include "_Module/API.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/Math/Quaternion.h"
#include "System/Core/Types/BitFlags.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API Blender
    {
    public:

        enum class PoseBlendOptions
        {
            None = 0,
            Interpolate,
            Additive,
            GlobalSpace,
            LockUnmaskedBones,
            LayeredBlend,
        };

        enum class RootMotionBlendOptions
        {
            None = 0,
            Additive,
            IgnoreSource,
            IgnoreTarget
        };

    private:

        struct InterpolativeBlender
        {
            inline static Quaternion BlendRotation( Quaternion const& quat0, Quaternion const& quat1, float t )
            {
                return Quaternion::SLerp( quat0, quat1, t );
            }

            inline static Vector BlendTranslation( Vector const& trans0, Vector const& trans1, float t )
            {
                return Vector::Lerp( trans0, trans1, t );
            }

            inline static Vector BlendScale( Vector const& scale0, Vector const& scale1, float t )
            {
                return Vector::Lerp( scale0, scale1, t );
            }
        };

        struct AdditiveBlender
        {
            inline static Quaternion BlendRotation( Quaternion const& quat0, Quaternion const& quat1, float t )
            {
                Quaternion const targetQuat = quat0 * quat1;
                return Quaternion::SLerp( quat0, targetQuat, t );
            }

            inline static Vector BlendTranslation( Vector const& trans0, Vector const& trans1, float t )
            {
                return Vector::MultiplyAdd( trans1, Vector( t ), trans0 ).SetW1();
            }

            inline static Vector BlendScale( Vector const& scale0, Vector const& scale1, float t )
            {
                return Vector::MultiplyAdd( scale1, Vector( t ), scale0 ).SetW1();
            }
        };

        struct BlendWeight
        {
            inline static float GetBlendWeight( float const blendWeight, BoneMask const* pBoneMask, int32 const boneIdx )
            {
                return blendWeight;
            }
        };

        struct BoneWeight
        {
            inline static float GetBlendWeight( float const blendWeight, BoneMask const* pBoneMask, int32 const boneIdx )
            {
                return blendWeight * pBoneMask->GetWeight( boneIdx );
            }
        };

    public:

        static void Blend( Pose const* pSourcePose, Pose const* pTargetPose, float blendWeight, TBitFlags<PoseBlendOptions> blendOptions, BoneMask const* pBoneMask, Pose* pResultPose );

        //-------------------------------------------------------------------------

        inline static Transform BlendRootMotionDeltas( Transform const& source, Transform const& target, float blendWeight, TBitFlags<RootMotionBlendOptions> blendOptions )
        {
            Transform result;

            if ( blendWeight <= 0.0f || blendOptions.IsFlagSet( RootMotionBlendOptions::IgnoreTarget ) )
            {
                result = source;
            }
            else if ( blendWeight >= 1.0f || blendOptions.IsFlagSet( RootMotionBlendOptions::IgnoreSource ) )
            {
                result = target;
            }
            else
            {
                if ( blendOptions.IsFlagSet( RootMotionBlendOptions::Additive ) )
                {
                    result.SetRotation( AdditiveBlender::BlendRotation( source.GetRotation(), target.GetRotation(), blendWeight ) );
                    result.SetTranslation( AdditiveBlender::BlendTranslation( source.GetTranslation(), target.GetTranslation(), blendWeight ) );
                    result.SetScale( Vector::One );
                }
                else
                {
                    result.SetRotation( InterpolativeBlender::BlendRotation( source.GetRotation(), target.GetRotation(), blendWeight ) );
                    result.SetTranslation( InterpolativeBlender::BlendTranslation( source.GetTranslation(), target.GetTranslation(), blendWeight ).SetW1() );
                    result.SetScale( Vector::One );
                }
            }

            return result;
        }
    };
}