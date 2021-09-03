#include "FbxAnimation.h"
#include "FbxSceneContext.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    class FbxRawAnimation : public RawAnimation
    {
        friend class FbxAnimationFileReader;
    };

    //-------------------------------------------------------------------------

    class FbxAnimationFileReader
    {

    public:

        static TUniquePtr<KRG::RawAssets::RawAnimation> ReadAnimation( FileSystem::Path const& sourceFilePath, RawSkeleton const& rawSkeleton, String const& animationName )
        {
            KRG_ASSERT( sourceFilePath.IsValid() && rawSkeleton.IsValid() );

            TUniquePtr<RawAnimation> pAnimation( KRG::New<RawAnimation>( rawSkeleton ) );
            FbxRawAnimation* pRawAnimation = (FbxRawAnimation*) pAnimation.get();

            Fbx::FbxSceneContext sceneCtx( sourceFilePath );
            if ( sceneCtx.IsValid() )
            {
                // Find the required anim stack
                //-------------------------------------------------------------------------

                FbxAnimStack* pAnimStack = nullptr;
                if ( !animationName.empty() )
                {
                    pAnimStack = sceneCtx.FindAnimStack( animationName.c_str() );
                    if ( pAnimStack == nullptr )
                    {
                        pRawAnimation->LogError( "Could not find requested animation (%s) in FBX scene", animationName.c_str() );
                        return pAnimation;
                    }
                }
                else // Take the first anim stack present
                {
                    auto const numAnimStacks = sceneCtx.m_pScene->GetSrcObjectCount<FbxAnimStack>();
                    if ( numAnimStacks == 0 )
                    {
                        pRawAnimation->LogError( "No animations found in the FBX scene", animationName.c_str() );
                        return pAnimation;
                    }

                    pAnimStack = sceneCtx.m_pScene->GetSrcObject<FbxAnimStack>( 0 );
                }

                KRG_ASSERT( pAnimStack != nullptr );

                // Read animation data
                //-------------------------------------------------------------------------

                sceneCtx.m_pScene->SetCurrentAnimationStack( pAnimStack );

                // Read animation start and end times
                FbxTime duration;
                FbxTakeInfo const* pTakeInfo = sceneCtx.m_pScene->GetTakeInfo( pAnimStack->GetNameWithoutNameSpacePrefix() );
                if ( pTakeInfo != nullptr )
                {
                    pRawAnimation->m_start = (float) pTakeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
                    pRawAnimation->m_end = (float) pTakeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
                    duration = pTakeInfo->mLocalTimeSpan.GetDuration();
                }
                else // Take the time line value
                {
                    FbxTimeSpan timeLineSpan;
                    sceneCtx.m_pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan( timeLineSpan );
                    pRawAnimation->m_start = (float) timeLineSpan.GetStart().GetSecondDouble();
                    pRawAnimation->m_end = (float) timeLineSpan.GetStop().GetSecondDouble();
                    duration = timeLineSpan.GetDuration();
                }

                // Calculate frame rate
                FbxTime::EMode mode = duration.GetGlobalTimeMode();
                double frameRate = duration.GetFrameRate( mode );

                // Set sampling rate and allocate memory
                pRawAnimation->m_samplingFrameRate = (float) duration.GetFrameRate( mode );
                float const samplingTimeStep = 1.0f / pRawAnimation->m_samplingFrameRate;
                pRawAnimation->m_numFrames = (uint32) Math::Floor( pRawAnimation->GetDuration() / samplingTimeStep ) + 1;

                // Read animation data
                //-------------------------------------------------------------------------

                ReadTrackData( sceneCtx, *pRawAnimation );
            }
            else
            {
                pRawAnimation->LogError( "Failed to read FBX file: %s -> %s", sourceFilePath.c_str(), sceneCtx.GetErrorMessage().c_str() );
            }

            return pAnimation;
        }

        static void ReadTrackData( Fbx::FbxSceneContext const& sceneCtx, FbxRawAnimation& rawAnimation )
        {
            int32 const numBones = rawAnimation.m_skeleton.GetNumBones();
            float const samplingTimeStep = 1.0f / rawAnimation.m_samplingFrameRate;
            uint32 const maxKeys = rawAnimation.m_numFrames * 3;

            rawAnimation.m_tracks.resize( numBones );

            // Read global transforms
            //-------------------------------------------------------------------------
            // We need to read the global transforms so as to take into account any scene conversion operations applied

            FbxAnimEvaluator* pEvaluator = sceneCtx.m_pScene->GetAnimationEvaluator();
            for ( auto boneIdx = 0; boneIdx < numBones; boneIdx++ )
            {
                StringID const& boneName = rawAnimation.m_skeleton.GetBoneName( boneIdx );
                RawAnimation::TrackData& animTrack = rawAnimation.m_tracks[boneIdx];
                FbxNode* pBoneNode = sceneCtx.m_pScene->FindNodeByName( boneName.c_str() );

                // Get the parent node for non-root bones
                FbxNode* pParentBoneNode = nullptr;
                if ( boneIdx != 0 )
                {
                    int32 const parentBoneIdx = rawAnimation.m_skeleton.GetParentBoneIndex( boneIdx );
                    KRG_ASSERT( parentBoneIdx != InvalidIndex );
                    StringID const parentBoneName = rawAnimation.m_skeleton.GetBoneName( parentBoneIdx );
                    pParentBoneNode = sceneCtx.m_pScene->FindNodeByName( parentBoneName.c_str() );
                    KRG_ASSERT( pParentBoneNode != nullptr );
                }

                // Find a node that matches skeleton joint
                if ( pBoneNode == nullptr )
                {
                    rawAnimation.LogWarning( "Warning: No animation track found for bone (%s), Using skeleton bind pose instead.", boneName.c_str() );

                    // Store skeleton reference pose for bone
                    for ( auto i = 0u; i < rawAnimation.m_numFrames; i++ )
                    {
                        animTrack.m_transforms.emplace_back( rawAnimation.m_skeleton.GetLocalTransform( boneIdx ) );
                    }
                }
                else
                {
                    // Reserve keys in animation tracks
                    animTrack.m_transforms.reserve( maxKeys );

                    // Sample animation data
                    float currentTime = rawAnimation.m_start;
                    for ( auto l = 0u; l < rawAnimation.m_numFrames; l++, currentTime += samplingTimeStep )
                    {
                        // Root bone is already in local space
                        if ( boneIdx == 0 )
                        {
                            FbxAMatrix nodeGlobalTransform = pEvaluator->GetNodeGlobalTransform( pBoneNode, FbxTimeSeconds( currentTime ) );
                            animTrack.m_transforms.emplace_back( sceneCtx.ConvertMatrixToTransform( nodeGlobalTransform ) );
                        }
                        else // Read the global transforms and convert to local
                        {
                            FbxAMatrix const nodeParentGlobalTransform = pEvaluator->GetNodeGlobalTransform( pParentBoneNode, FbxTimeSeconds( currentTime ) );
                            FbxAMatrix const nodeGlobalTransform = pEvaluator->GetNodeGlobalTransform( pBoneNode, FbxTimeSeconds( currentTime ) );
                            FbxAMatrix const nodeLocalTransform = nodeParentGlobalTransform.Inverse() * nodeGlobalTransform;

                            animTrack.m_transforms.emplace_back( sceneCtx.ConvertMatrixToTransform( nodeLocalTransform ) );
                        }
                    }

                    // Update the end duration to the actual sampled end time
                    rawAnimation.m_end = currentTime;
                }
            }
        }
    };
}

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    TUniquePtr<KRG::RawAssets::RawAnimation> ReadAnimation( FileSystem::Path const& animationFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& takeName )
    {
        return RawAssets::FbxAnimationFileReader::ReadAnimation( animationFilePath, rawSkeleton, takeName );
    }
}