#pragma once

#include "../_Module/API.h"
#include "System/Animation/AnimationBoneMask.h"
#include "System/Animation/AnimationPose.h"
#include "System/Core/Math/Quaternion.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        struct KRG_ENGINE_ANIMATION_API Blender
        {
            struct Options
            {
                enum Values
                {
                    None = 0,
                    Interpolate = 1 << 0,
                    Additive = 1 << 1,
                    GlobalSpace = 1 << 2,
                    LockUnmaskedBones = 1 << 3,
                    LayeredBlend = 1 << 4,
                };
            };

            //-------------------------------------------------------------------------

            struct InterpolativeBlender
            {
                inline static Quaternion BlendRotation( Quaternion const& quat0, Quaternion const& quat1, F32 t )
                {
                    return Quaternion::SLerp( quat0, quat1, t );
                }

                inline static Vector BlendTranslation( Vector const& trans0, Vector const& trans1, F32 t )
                {
                    return Vector::Lerp( trans0, trans1, t );
                }
            };

            struct AdditiveBlender
            {
                inline static Quaternion BlendRotation( Quaternion const& quat0, Quaternion const& quat1, F32 t )
                {
                    auto const targetQuat = quat0 * quat1;
                    return Quaternion::SLerp( quat0, targetQuat, t );
                }

                inline static Vector BlendTranslation( Vector const& trans0, Vector const& trans1, F32 t )
                {
                    return Vector::MultiplyAdd( trans1, Vector( t ), trans0 ).SetW1();
                }
            };

            struct BlendWeight
            {
                inline static F32 GetBlendWeight( F32 const blendWeight, BoneMask const* pBoneMask, S32 const boneIdx )
                {
                    return blendWeight;
                }
            };

            struct BoneWeight
            {
                inline static F32 GetBlendWeight( F32 const blendWeight, BoneMask const* pBoneMask, S32 const boneIdx )
                {
                    return blendWeight * pBoneMask->GetWeight( boneIdx );
                }
            };

            //-------------------------------------------------------------------------

            static void Blend( Pose const* pSourcePose, Pose const* pTargetPose, F32 blendWeight, U32 blendOptions, BoneMask const* pBoneMask, Pose* pResultPose );

            //-------------------------------------------------------------------------

            inline static Transform BlendRootMotionDeltas( Transform const& from, Transform const& to, F32 blendWeight )
            {
                if ( blendWeight <= 0.0f )
                {
                    return from;
                }
                else if ( blendWeight >= 1.0f )
                {
                    return to;
                }
                else
                {
                    Quaternion const rotation = Quaternion::SLerp( from.GetRotation(), to.GetRotation(), blendWeight );
                    Vector const translation = Vector::SLerp( from.GetTranslation(), to.GetTranslation(), blendWeight ).SetW1();
                    return Transform( rotation, translation );
                }
            }
        };
    }
}