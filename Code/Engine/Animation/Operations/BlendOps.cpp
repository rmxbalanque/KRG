#include "BlendOps.h"
#include "System/Core/Types/BitFlags.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        template<typename Blender, typename BlendWeight>
        void BlenderLocal( Pose const* pSourcePose, Pose const* pTargetPose, float const blendWeight, BoneMask const* pBoneMask, Pose* pResultPose )
        {
            KRG_ASSERT( blendWeight >= 0.0f && blendWeight <= 1.0f );
            KRG_ASSERT( pSourcePose != nullptr && pTargetPose != nullptr && pResultPose != nullptr );

            if ( pBoneMask != nullptr )
            {
                KRG_ASSERT( pBoneMask->GetNumWeights() == pSourcePose->GetSkeleton()->GetNumBones() );
            }

            int32 const numBones = pResultPose->GetNumBones();
            for ( int32 boneIdx = 0; boneIdx < numBones; boneIdx++ )
            {
                // If the bone has been masked out
                float const boneBlendWeight = BlendWeight::GetBlendWeight( blendWeight, pBoneMask, boneIdx );
                if ( boneBlendWeight == 0.0f )
                {
                    pResultPose->SetTransform( boneIdx, pSourcePose->GetTransform( boneIdx ) );
                }
                else // Perform Blend
                {
                    // Blend translations
                    Vector const translation = Blender::BlendTranslation( pSourcePose->GetTransform( boneIdx ).GetTranslation(), pTargetPose->GetTransform( boneIdx ).GetTranslation(), boneBlendWeight );
                    pResultPose->SetTranslation( boneIdx, translation );

                    // Blend rotations
                    Quaternion const rotation = Blender::BlendRotation( pSourcePose->GetTransform( boneIdx ).GetRotation(), pTargetPose->GetTransform( boneIdx ).GetRotation(), boneBlendWeight );
                    pResultPose->SetRotation( boneIdx, rotation );
                }
            }
        }

        //-------------------------------------------------------------------------

        template<typename Blender, typename BlendWeight>
        void BlenderGlobal( Pose const* pSourcePose, Pose const* pTargetPose, float const blendWeight, BoneMask const* pBoneMask, Pose* pResultPose )
        {
            static auto const rootBoneIndex = 0;
            KRG_ASSERT( blendWeight >= 0.0f && blendWeight <= 1.0f );
            KRG_ASSERT( pSourcePose != nullptr && pTargetPose != nullptr && pResultPose != nullptr );

            if ( pBoneMask != nullptr )
            {
                KRG_ASSERT( pBoneMask->GetNumWeights() == pSourcePose->GetSkeleton()->GetNumBones() );
            }

            // Blend the root separately - local space blend
            //-------------------------------------------------------------------------

            auto boneBlendWeight = BlendWeight::GetBlendWeight( blendWeight, pBoneMask, rootBoneIndex );
            if ( boneBlendWeight != 0.0f )
            {
                Vector const translation = Blender::BlendTranslation( pSourcePose->GetTransform( rootBoneIndex ).GetTranslation(), pTargetPose->GetTransform( rootBoneIndex ).GetTranslation(), boneBlendWeight );
                pResultPose->SetTranslation( rootBoneIndex, translation );

                Quaternion const rotation = Blender::BlendRotation( pSourcePose->GetTransform( rootBoneIndex ).GetRotation(), pTargetPose->GetTransform( rootBoneIndex ).GetRotation(), boneBlendWeight );
                pResultPose->SetRotation( rootBoneIndex, rotation );
            }

            // Blend global space poses together and convert back to local space
            //-------------------------------------------------------------------------

            auto const parentIndices = pSourcePose->GetSkeleton()->GetParentIndices();
            auto const numBones = pResultPose->GetNumBones();
            for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
            {
                boneBlendWeight = BlendWeight::GetBlendWeight( blendWeight, pBoneMask, boneIdx );

                if ( boneBlendWeight == 0.0f )
                {
                    pResultPose->SetTransform( boneIdx, pSourcePose->GetTransform( boneIdx ) );
                }
                else // Perform Blend
                {
                    // Blend translations - translation blending is done in local space
                    Vector const translation = Blender::BlendTranslation( pSourcePose->GetTransform( boneIdx ).GetTranslation(), pTargetPose->GetTransform( boneIdx ).GetTranslation(), boneBlendWeight );
                    pResultPose->SetTranslation( boneIdx, translation );

                    auto const parentIdx = parentIndices[boneIdx];
                    auto const parentBoneBlendWeight = BlendWeight::GetBlendWeight( blendWeight, pBoneMask, parentIdx );

                    // If the bone weights are the same i.e. inherited, then perform a local space blend
                    if ( Math::IsNearEqual( boneBlendWeight, parentBoneBlendWeight ) )
                    {
                        Quaternion const rotation = Blender::BlendRotation( pSourcePose->GetTransform( boneIdx ).GetRotation(), pTargetPose->GetTransform( boneIdx ).GetRotation(), boneBlendWeight );
                        pResultPose->SetRotation( boneIdx, rotation );
                    }
                    else // Perform a global space blend for this joS32
                    {
                        // TODO: If the calls to GetGlobalSpaceTransform() get expensive optimize this by creating a global space bone cache
                        Transform const sourceGlobalTransform = pSourcePose->GetGlobalTransform( boneIdx );
                        Transform const targetGlobalTransform = pTargetPose->GetGlobalTransform( boneIdx );
                        Quaternion const rotation = Blender::BlendRotation( sourceGlobalTransform.GetRotation(), targetGlobalTransform.GetRotation(), boneBlendWeight );

                        // Convert blended global space rotation to local space for the result pose
                        // Note: our quaternion inverse function ONLY works on unit quaternions so we need to ensure we normalize the parent before inverting
                        Quaternion const parentRotation = pResultPose->GetGlobalTransform( parentIdx ).GetRotation();
                        Quaternion const localRotation = parentRotation.GetConjugate() * rotation;
                        pResultPose->SetRotation( boneIdx, localRotation );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        namespace
        {
            static void LockUnmaskedBones( Pose const* pSourcePose, BoneMask const* pBoneMask, Pose* pResultPose )
            {
                // If these poses are the same, this code will do nothing as the pResult->CalculatGlobalPose will overwrite the original transforms
                KRG_ASSERT( pSourcePose != pResultPose );

                // Get global transforms from the source and the target pose
                auto const& sourceGlobalTransforms = pSourcePose->GetGlobalTransforms();
                KRG_ASSERT( sourceGlobalTransforms.size() == pSourcePose->GetNumBones() );

                pResultPose->CalculateGlobalTransforms();
                auto& resultGlobalTransforms = pResultPose->GetGlobalTransforms();

                // Copy global transforms from the original pose
                int32 const numBones = pResultPose->GetNumBones();
                for ( int32 boneIdx = 1; boneIdx < numBones; boneIdx++ )
                {
                    auto const boneBlendWeight = pBoneMask->GetWeight( boneIdx );
                    if ( boneBlendWeight == 0.0f )
                    {
                        pResultPose->SetTransform( boneIdx, sourceGlobalTransforms[boneIdx] );
                    }
                }

                // Calculate new local transforms
                auto pSkeleton = pResultPose->GetSkeleton();
                for ( auto boneIdx = ( numBones - 1 ); boneIdx > 0; boneIdx-- )
                {
                    auto const parentBoneIdx = pSkeleton->GetParentIndex( boneIdx );
                    Transform const localTransform = resultGlobalTransforms[boneIdx] * resultGlobalTransforms[parentBoneIdx].GetInverse();
                    pResultPose->SetTransform( boneIdx, localTransform );
                }
            }
        }

        //-------------------------------------------------------------------------

        void Blender::Blend( Pose const* pSourcePose, Pose const* pTargetPose, float const blendWeight, uint32 const blendOptions, BoneMask const* pBoneMask, Pose* pResultPose )
        {
            TBitFlags<Options::Values> optionFlags( blendOptions );

            pResultPose->ClearGlobalTransforms();

            if ( pBoneMask == nullptr )
            {
                if ( optionFlags.IsFlagSet( Options::GlobalSpace ) )
                {
                    if ( optionFlags.IsFlagSet( Options::Interpolate ) )
                    {
                        BlenderGlobal<InterpolativeBlender, BlendWeight>( pSourcePose, pTargetPose, blendWeight, nullptr, pResultPose );
                    }
                    else if ( optionFlags.IsFlagSet( Options::Additive ) )
                    {
                        BlenderGlobal<AdditiveBlender, BlendWeight>( pSourcePose, pTargetPose, blendWeight, nullptr, pResultPose );
                    }
                }
                else
                {
                    if ( optionFlags.IsFlagSet( Options::Interpolate ) )
                    {
                        BlenderLocal<InterpolativeBlender, BlendWeight>( pSourcePose, pTargetPose, blendWeight, nullptr, pResultPose );
                    }
                    else if ( optionFlags.IsFlagSet( Options::Additive ) )
                    {
                        BlenderLocal<AdditiveBlender, BlendWeight>( pSourcePose, pTargetPose, blendWeight, nullptr, pResultPose );
                    }
                }
            }
            else // We have a bone mask set
            {
                if ( optionFlags.IsFlagSet( Options::GlobalSpace ) )
                {
                    KRG_ASSERT( !pResultPose->HasGlobalTransforms() ); // Ensure we dont have any cached transforms that might screw up the local space transformation

                    if ( optionFlags.IsFlagSet( Options::Interpolate ) )
                    {
                        BlenderGlobal<InterpolativeBlender, BoneWeight>( pSourcePose, pTargetPose, blendWeight, pBoneMask, pResultPose );
                    }
                    else if ( optionFlags.IsFlagSet( Options::Additive ) )
                    {
                        BlenderGlobal<AdditiveBlender, BoneWeight>( pSourcePose, pTargetPose, blendWeight, pBoneMask, pResultPose );
                    }
                }
                else
                {
                    if ( optionFlags.IsFlagSet( Options::Interpolate ) )
                    {
                        BlenderLocal<InterpolativeBlender, BoneWeight>( pSourcePose, pTargetPose, blendWeight, pBoneMask, pResultPose );
                    }
                    else if ( optionFlags.IsFlagSet( Options::Additive ) )
                    {
                        BlenderLocal<AdditiveBlender, BoneWeight>( pSourcePose, pTargetPose, blendWeight, pBoneMask, pResultPose );
                    }
                }

                // Ensure that the unmasked bones maS32ain their global transforms
                if ( optionFlags.IsFlagSet( Options::LockUnmaskedBones ) )
                {
                    LockUnmaskedBones( pSourcePose, pBoneMask, pResultPose );
                }
            }
        }
    }
}