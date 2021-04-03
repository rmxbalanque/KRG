#ifdef _WIN32
#include "AnimationCompiler.h"
#include "Compilers/Animation/Skeleton/AnimSkeletonCompiler.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Resource/RawAssets/RawAnimation.h"
#include "Engine/Animation/AnimationData.h"
#include "Engine/Animation/AnimationPose.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        AnimationCompiler::AnimationCompiler()
            : Resource::Compiler( "AnimationCompiler", VERSION )
        {
            m_outputTypes.push_back( AnimationData::GetStaticResourceTypeID() );
        }

        Resource::CompilationResult AnimationCompiler::Compile( Resource::CompileContext const& ctx ) const
        {
            AnimationResourceDescriptor resourceDescriptor;
            if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
            {
                return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            // Read Skeleton Data
            //-------------------------------------------------------------------------

            // Convert the skeleton resource path to a physical file path
            if ( !resourceDescriptor.m_pSkeleton.GetResourceID().IsValid() )
            {
                return Error( "Invalid skeleton resource ID" );
            }

            DataPath const& skeletonDataPath = resourceDescriptor.m_pSkeleton.GetResourceID().GetDataPath();
            FileSystem::Path skeletonDescriptorFilePath;
            if ( !ctx.ConvertDataPathToFilePath( skeletonDataPath, skeletonDescriptorFilePath ) )
            {
                return Error( "Invalid skeleton data path: %s", skeletonDataPath.c_str() );
            }

            if ( !skeletonDescriptorFilePath.Exists() )
            {
                return Error( "Invalid skeleton descriptor file path: %s", skeletonDescriptorFilePath.ToString().c_str() );
            }

            SkeletonResourceDescriptor skeletonResourceDescriptor;
            if ( !ctx.TryReadResourceDescriptorFromFile( skeletonDescriptorFilePath, skeletonResourceDescriptor ) )
            {
                return Error( "Failed to read skeleton resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            FileSystem::Path skeletonFilePath;
            if ( !ctx.ConvertDataPathToFilePath( skeletonResourceDescriptor.m_skeletonDataPath, skeletonFilePath ) )
            {
                return Error( "Invalid skeleton FBX data path: %s", skeletonResourceDescriptor.m_skeletonDataPath.ToString().c_str() );
            }

            RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
            auto pRawSkeleton = RawAssets::ReadSkeleton( readerCtx, skeletonFilePath, skeletonResourceDescriptor.m_skeletonRootBoneName );
            if ( pRawSkeleton == nullptr && !pRawSkeleton->IsValid() )
            {
                return Error( "Failed to read skeleton file: %s", skeletonFilePath.ToString().c_str() );
            }

            // Read animation data
            //-------------------------------------------------------------------------

            FileSystem::Path animationFilePath;
            if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_animationDataPath, animationFilePath ) )
            {
                return Error( "Invalid animation data path: %s", resourceDescriptor.m_animationDataPath.c_str() );
            }

            if ( !animationFilePath.Exists() )
            {
                return Error( "Invalid animation file path: %s", animationFilePath.ToString().c_str() );
            }

            TUniquePtr<RawAssets::RawAnimation> pRawAnimation = RawAssets::ReadAnimation( readerCtx, animationFilePath, *pRawSkeleton, resourceDescriptor.m_animationName );
            if ( pRawAnimation == nullptr )
            {
                return Error( "Failed to read animation from source file" );
            }

            // Reflect FBX data into runtime format
            //-------------------------------------------------------------------------

            AnimationData animData;
            animData.m_pSkeleton = resourceDescriptor.m_pSkeleton;
            TransferAndCompressAnimationData( *pRawAnimation, animData );

            // Serialize animation data
            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                Resource::ResourceHeader hdr( VERSION, resourceDescriptor.m_resourceTypeID );
                hdr.AddInstallDependency( resourceDescriptor.m_pSkeleton.GetResourceID() );
                archive << hdr << animData;

                if( pRawAnimation->HasWarnings() )
                {
                    return CompilationSucceededWithWarnings( ctx );
                }
                else
                {
                    return CompilationSucceeded( ctx );
                }
            }
            else
            {
                return CompilationFailed( ctx );
            }
        }

        //-------------------------------------------------------------------------

        void AnimationCompiler::TransferAndCompressAnimationData( RawAssets::RawAnimation const& rawAnimData, AnimationData& animData ) const
        {
            auto const& rawTrackData = rawAnimData.GetTrackData();
            int32 const numBones = rawAnimData.GetNumBones();

            // Transfer basic anim data
            //-------------------------------------------------------------------------

            animData.m_numFrames = rawAnimData.GetNumFrames();
            animData.m_duration = ( animData.IsSingleFrameAnimation() ) ? 0.0f : rawAnimData.GetDuration();

            // Compress raw data
            //-------------------------------------------------------------------------

            static constexpr float const defaultQuantizationRangeLength = 0.1f;

            for ( int32 boneIdx = 0; boneIdx < numBones; boneIdx++ )
            {
                TrackCompressionSettings trackSettings;

                // Record offset into data for this track
                trackSettings.m_trackStartIndex = (uint32) animData.m_compressedPoseData.size();

                //-------------------------------------------------------------------------
                // Rotation
                //-------------------------------------------------------------------------

                for ( uint32 frameIdx = 0; frameIdx < animData.m_numFrames; frameIdx++ )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                    Quaternion const rotation = rawBoneTransform.GetRotation();

                    Quantization::EncodedQuaternion const encodedQuat( rotation );
                    animData.m_compressedPoseData.push_back( encodedQuat.GetData0() );
                    animData.m_compressedPoseData.push_back( encodedQuat.GetData1() );
                    animData.m_compressedPoseData.push_back( encodedQuat.GetData2() );
                }

                //-------------------------------------------------------------------------
                // Translation
                //-------------------------------------------------------------------------

                auto const& rawTranslationValueRangeX = rawTrackData[boneIdx].m_translationValueRangeX;
                auto const& rawTranslationValueRangeY = rawTrackData[boneIdx].m_translationValueRangeY;
                auto const& rawTranslationValueRangeZ = rawTrackData[boneIdx].m_translationValueRangeZ;

                float const& rawTranslationValueRangeLengthX = rawTranslationValueRangeX.GetLength();
                float const& rawTranslationValueRangeLengthY = rawTranslationValueRangeY.GetLength();
                float const& rawTranslationValueRangeLengthZ = rawTranslationValueRangeZ.GetLength();

                // We could arguably compress more by saving each component individually at the cost of sampling performance. If we absolutely need more compression, we can do it here
                bool const isTranslationConstant = Math::IsNearlyZero( rawTranslationValueRangeLengthX ) && Math::IsNearlyZero( rawTranslationValueRangeLengthY ) && Math::IsNearlyZero( rawTranslationValueRangeLengthZ );
                if ( isTranslationConstant )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                    Vector const& translation = rawBoneTransform.GetTranslation();

                    trackSettings.m_translationRangeX = { translation.m_x, defaultQuantizationRangeLength };
                    trackSettings.m_translationRangeY = { translation.m_y, defaultQuantizationRangeLength };
                    trackSettings.m_translationRangeZ = { translation.m_z, defaultQuantizationRangeLength };
                    trackSettings.m_isTranslationStatic = true;
                }
                else
                {
                    trackSettings.m_translationRangeX = { rawTranslationValueRangeX.m_min, rawTranslationValueRangeLengthX };
                    trackSettings.m_translationRangeY = { rawTranslationValueRangeY.m_min, rawTranslationValueRangeLengthY };
                    trackSettings.m_translationRangeZ = { rawTranslationValueRangeZ.m_min, rawTranslationValueRangeLengthZ };
                }

                //-------------------------------------------------------------------------

                if ( trackSettings.IsTranslationTrackStatic() )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                    Vector const& translation = rawBoneTransform.GetTranslation();

                    uint16 const m_x = Quantization::EncodeFloat( translation.m_x, trackSettings.m_translationRangeX.m_rangeStart, trackSettings.m_translationRangeX.m_rangeLength );
                    uint16 const m_y = Quantization::EncodeFloat( translation.m_y, trackSettings.m_translationRangeY.m_rangeStart, trackSettings.m_translationRangeY.m_rangeLength );
                    uint16 const m_z = Quantization::EncodeFloat( translation.m_z, trackSettings.m_translationRangeZ.m_rangeStart, trackSettings.m_translationRangeZ.m_rangeLength );

                    animData.m_compressedPoseData.push_back( m_x );
                    animData.m_compressedPoseData.push_back( m_y );
                    animData.m_compressedPoseData.push_back( m_z );
                }
                else // Store all frames
                {
                    for ( uint32 frameIdx = 0; frameIdx < animData.m_numFrames; frameIdx++ )
                    {
                        Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                        Vector const& translation = rawBoneTransform.GetTranslation();

                        uint16 const m_x = Quantization::EncodeFloat( translation.m_x, trackSettings.m_translationRangeX.m_rangeStart, trackSettings.m_translationRangeX.m_rangeLength );
                        uint16 const m_y = Quantization::EncodeFloat( translation.m_y, trackSettings.m_translationRangeY.m_rangeStart, trackSettings.m_translationRangeY.m_rangeLength );
                        uint16 const m_z = Quantization::EncodeFloat( translation.m_z, trackSettings.m_translationRangeZ.m_rangeStart, trackSettings.m_translationRangeZ.m_rangeLength );

                        animData.m_compressedPoseData.push_back( m_x );
                        animData.m_compressedPoseData.push_back( m_y );
                        animData.m_compressedPoseData.push_back( m_z );
                    }
                }

                //-------------------------------------------------------------------------
                // Scale
                //-------------------------------------------------------------------------
                
                auto const& rawScaleValueRangeX = rawTrackData[boneIdx].m_scaleValueRangeX;
                auto const& rawScaleValueRangeY = rawTrackData[boneIdx].m_scaleValueRangeY;
                auto const& rawScaleValueRangeZ = rawTrackData[boneIdx].m_scaleValueRangeZ;

                float const& rawScaleValueRangeLengthX = rawScaleValueRangeX.GetLength();
                float const& rawScaleValueRangeLengthY = rawScaleValueRangeY.GetLength();
                float const& rawScaleValueRangeLengthZ = rawScaleValueRangeZ.GetLength();

                // We could arguably compress more by saving each component individually at the cost of sampling performance. If we absolutely need more compression, we can do it here
                bool const isScaleConstant = Math::IsNearlyZero( rawScaleValueRangeLengthX ) && Math::IsNearlyZero( rawScaleValueRangeLengthY ) && Math::IsNearlyZero( rawScaleValueRangeLengthZ );
                if ( isScaleConstant )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                    Vector const& scale = rawBoneTransform.GetScale();

                    trackSettings.m_scaleRangeX = { scale.m_x, defaultQuantizationRangeLength };
                    trackSettings.m_scaleRangeY = { scale.m_y, defaultQuantizationRangeLength };
                    trackSettings.m_scaleRangeZ = { scale.m_z, defaultQuantizationRangeLength };
                    trackSettings.m_isScaleStatic = true;
                }
                else
                {
                    trackSettings.m_scaleRangeX = { rawScaleValueRangeX.m_min, rawScaleValueRangeLengthX };
                    trackSettings.m_scaleRangeY = { rawScaleValueRangeY.m_min, rawScaleValueRangeLengthY };
                    trackSettings.m_scaleRangeZ = { rawScaleValueRangeZ.m_min, rawScaleValueRangeLengthZ };
                }

                //-------------------------------------------------------------------------

                if ( trackSettings.IsScaleTrackStatic() )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                    Vector const& scale = rawBoneTransform.GetScale();

                    uint16 const m_x = Quantization::EncodeFloat( scale.m_x, trackSettings.m_scaleRangeX.m_rangeStart, trackSettings.m_scaleRangeX.m_rangeLength );
                    uint16 const m_y = Quantization::EncodeFloat( scale.m_y, trackSettings.m_scaleRangeY.m_rangeStart, trackSettings.m_scaleRangeY.m_rangeLength );
                    uint16 const m_z = Quantization::EncodeFloat( scale.m_z, trackSettings.m_scaleRangeZ.m_rangeStart, trackSettings.m_scaleRangeZ.m_rangeLength );

                    animData.m_compressedPoseData.push_back( m_x );
                    animData.m_compressedPoseData.push_back( m_y );
                    animData.m_compressedPoseData.push_back( m_z );
                }
                else // Store all frames
                {
                    for ( uint32 frameIdx = 0; frameIdx < animData.m_numFrames; frameIdx++ )
                    {
                        Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                        Vector const& scale = rawBoneTransform.GetScale();

                        uint16 const m_x = Quantization::EncodeFloat( scale.m_x, trackSettings.m_scaleRangeX.m_rangeStart, trackSettings.m_scaleRangeX.m_rangeLength );
                        uint16 const m_y = Quantization::EncodeFloat( scale.m_y, trackSettings.m_scaleRangeY.m_rangeStart, trackSettings.m_scaleRangeY.m_rangeLength );
                        uint16 const m_z = Quantization::EncodeFloat( scale.m_z, trackSettings.m_scaleRangeZ.m_rangeStart, trackSettings.m_scaleRangeZ.m_rangeLength );

                        animData.m_compressedPoseData.push_back( m_x );
                        animData.m_compressedPoseData.push_back( m_y );
                        animData.m_compressedPoseData.push_back( m_z );
                    }
                }

                //-------------------------------------------------------------------------

                animData.m_trackCompressionSettings.emplace_back( trackSettings );
            }
        }
    }
}
#endif