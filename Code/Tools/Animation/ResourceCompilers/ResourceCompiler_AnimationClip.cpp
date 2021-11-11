#include "ResourceCompiler_AnimationClip.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationClip.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Animation/Events/AnimationEventData.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Core/Resource/RawAssets/RawAnimation.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationClipCompiler::AnimationClipCompiler()
        : Resource::Compiler( "AnimationCompiler", s_version )
    {
        m_outputTypes.push_back( AnimationClip::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationClipCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        AnimationClipResourceDescriptor resourceDescriptor;

        TypeSystem::Serialization::TypeReader typeReader( ctx.m_typeRegistry );
        if ( !typeReader.ReadFromFile( ctx.m_inputFilePath ) )
        {
            return Error( "Failed to read resource descriptor file: %s", ctx.m_inputFilePath.c_str() );
        }

        if ( !typeReader.ReadType( &resourceDescriptor ) )
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

        ResourcePath const& skeletonPath = resourceDescriptor.m_pSkeleton.GetResourcePath();
        FileSystem::Path skeletonDescriptorFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( skeletonPath, skeletonDescriptorFilePath ) )
        {
            return Error( "Invalid skeleton data path: %s", skeletonPath.c_str() );
        }

        if ( !FileSystem::Exists( skeletonDescriptorFilePath ) )
        {
            return Error( "Invalid skeleton descriptor file path: %s", skeletonDescriptorFilePath.ToString().c_str() );
        }

        SkeletonResourceDescriptor skeletonResourceDescriptor;
        if ( !TryReadResourceDescriptorFromFile( ctx.m_typeRegistry, skeletonDescriptorFilePath, skeletonResourceDescriptor ) )
        {
            return Error( "Failed to read skeleton resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        FileSystem::Path skeletonFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( skeletonResourceDescriptor.m_skeletonPath, skeletonFilePath ) )
        {
            return Error( "Invalid skeleton FBX data path: %s", skeletonResourceDescriptor.m_skeletonPath.GetString().c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        auto pRawSkeleton = RawAssets::ReadSkeleton( readerCtx, skeletonFilePath, skeletonResourceDescriptor.m_skeletonRootBoneName );
        if ( pRawSkeleton == nullptr || !pRawSkeleton->IsValid() )
        {
            return Error( "Failed to read skeleton file: %s", skeletonFilePath.ToString().c_str() );
        }

        // Read animation data
        //-------------------------------------------------------------------------

        FileSystem::Path animationFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_animationPath, animationFilePath ) )
        {
            return Error( "Invalid animation data path: %s", resourceDescriptor.m_animationPath.c_str() );
        }

        if ( !FileSystem::Exists( animationFilePath ) )
        {
            return Error( "Invalid animation file path: %s", animationFilePath.ToString().c_str() );
        }

        TUniquePtr<RawAssets::RawAnimation> pRawAnimation = RawAssets::ReadAnimation( readerCtx, animationFilePath, *pRawSkeleton, resourceDescriptor.m_animationName );
        if ( pRawAnimation == nullptr )
        {
            return Error( "Failed to read animation from source file" );
        }

        // Reflect raw animation data into runtime format
        //-------------------------------------------------------------------------

        AnimationClip animData;
        animData.m_pSkeleton = resourceDescriptor.m_pSkeleton;
        TransferAndCompressAnimationData( *pRawAnimation, animData );

        // Handle events
        //-------------------------------------------------------------------------

        AnimationEventData eventData;
        if ( !ReadEventsData( ctx, typeReader.GetDocument(), *pRawAnimation, eventData ) )
        {
            return CompilationFailed( ctx );
        }

        // Serialize animation data
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, AnimationClip::GetStaticResourceTypeID() );
            hdr.AddInstallDependency( resourceDescriptor.m_pSkeleton.GetResourceID() );
            archive << hdr << animData;

            // Event Data
            archive << eventData.m_syncEventMarkers;
            archive << eventData.m_collection;

            if ( pRawAnimation->HasWarnings() )
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

    void AnimationClipCompiler::TransferAndCompressAnimationData( RawAssets::RawAnimation const& rawAnimData, AnimationClip& animClip ) const
    {
        auto const& rawTrackData = rawAnimData.GetTrackData();
        int32 const numBones = rawAnimData.GetNumBones();

        // Transfer basic anim data
        //-------------------------------------------------------------------------

        animClip.m_numFrames = rawAnimData.GetNumFrames();
        animClip.m_duration = ( animClip.IsSingleFrameAnimation() ) ? 0.0f : rawAnimData.GetDuration();
        animClip.m_rootMotionTrack = rawAnimData.GetRootMotion();

        // Calculate root motion extra data
        //-------------------------------------------------------------------------

        float totalDistance = 0.0f;
        float totalRotation = 0.0f;

        for ( auto i = 0u; i < animClip.GetNumFrames(); i++ )
        {
            // Track deltas
            if ( i > 0 )
            {
                Transform const deltaRoot = Transform::DeltaNoScale( animClip.m_rootMotionTrack[i - 1], animClip.m_rootMotionTrack[i] );
                totalDistance += deltaRoot.GetTranslation().GetLength3();

                // We use the negative world forward since deltas are relative to the identity transform
                Vector const deltaForward2D = deltaRoot.GetForwardVector().GetNormalized2();
                Radians const deltaAngle = Vector::GetAngleBetweenVectors( deltaForward2D, Vector::WorldBackward ).GetClamped();
                totalRotation += Math::Abs( deltaAngle );
            }
        }

        animClip.m_totalRootMotionDelta = Transform::DeltaNoScale( animClip.m_rootMotionTrack.front(), animClip.m_rootMotionTrack.back() );
        animClip.m_averageLinearVelocity = totalDistance / animClip.GetDuration();
        animClip.m_averageAngularVelocity = totalRotation / animClip.GetDuration();

        // Compress raw data
        //-------------------------------------------------------------------------

        static constexpr float const defaultQuantizationRangeLength = 0.1f;

        for ( int32 boneIdx = 0; boneIdx < numBones; boneIdx++ )
        {
            TrackCompressionSettings trackSettings;

            // Record offset into data for this track
            trackSettings.m_trackStartIndex = (uint32) animClip.m_compressedPoseData.size();

            //-------------------------------------------------------------------------
            // Rotation
            //-------------------------------------------------------------------------

            for ( uint32 frameIdx = 0; frameIdx < animClip.m_numFrames; frameIdx++ )
            {
                Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                Quaternion const rotation = rawBoneTransform.GetRotation();

                Quantization::EncodedQuaternion const encodedQuat( rotation );
                animClip.m_compressedPoseData.push_back( encodedQuat.GetData0() );
                animClip.m_compressedPoseData.push_back( encodedQuat.GetData1() );
                animClip.m_compressedPoseData.push_back( encodedQuat.GetData2() );
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
            bool const isTranslationConstant = Math::IsNearZero( rawTranslationValueRangeLengthX ) && Math::IsNearZero( rawTranslationValueRangeLengthY ) && Math::IsNearZero( rawTranslationValueRangeLengthZ );
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
                trackSettings.m_translationRangeX = { rawTranslationValueRangeX.m_start, Math::IsNearZero( rawTranslationValueRangeLengthX ) ? defaultQuantizationRangeLength : rawTranslationValueRangeLengthX };
                trackSettings.m_translationRangeY = { rawTranslationValueRangeY.m_start, Math::IsNearZero( rawTranslationValueRangeLengthY ) ? defaultQuantizationRangeLength : rawTranslationValueRangeLengthY };
                trackSettings.m_translationRangeZ = { rawTranslationValueRangeZ.m_start, Math::IsNearZero( rawTranslationValueRangeLengthZ ) ? defaultQuantizationRangeLength : rawTranslationValueRangeLengthZ };
            }

            //-------------------------------------------------------------------------

            if ( trackSettings.IsTranslationTrackStatic() )
            {
                Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                Vector const& translation = rawBoneTransform.GetTranslation();

                uint16 const m_x = Quantization::EncodeFloat( translation.m_x, trackSettings.m_translationRangeX.m_rangeStart, trackSettings.m_translationRangeX.m_rangeLength );
                uint16 const m_y = Quantization::EncodeFloat( translation.m_y, trackSettings.m_translationRangeY.m_rangeStart, trackSettings.m_translationRangeY.m_rangeLength );
                uint16 const m_z = Quantization::EncodeFloat( translation.m_z, trackSettings.m_translationRangeZ.m_rangeStart, trackSettings.m_translationRangeZ.m_rangeLength );

                animClip.m_compressedPoseData.push_back( m_x );
                animClip.m_compressedPoseData.push_back( m_y );
                animClip.m_compressedPoseData.push_back( m_z );
            }
            else // Store all frames
            {
                for ( uint32 frameIdx = 0; frameIdx < animClip.m_numFrames; frameIdx++ )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                    Vector const& translation = rawBoneTransform.GetTranslation();

                    uint16 const m_x = Quantization::EncodeFloat( translation.m_x, trackSettings.m_translationRangeX.m_rangeStart, trackSettings.m_translationRangeX.m_rangeLength );
                    uint16 const m_y = Quantization::EncodeFloat( translation.m_y, trackSettings.m_translationRangeY.m_rangeStart, trackSettings.m_translationRangeY.m_rangeLength );
                    uint16 const m_z = Quantization::EncodeFloat( translation.m_z, trackSettings.m_translationRangeZ.m_rangeStart, trackSettings.m_translationRangeZ.m_rangeLength );

                    animClip.m_compressedPoseData.push_back( m_x );
                    animClip.m_compressedPoseData.push_back( m_y );
                    animClip.m_compressedPoseData.push_back( m_z );
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
            bool const isScaleConstant = Math::IsNearZero( rawScaleValueRangeLengthX ) && Math::IsNearZero( rawScaleValueRangeLengthY ) && Math::IsNearZero( rawScaleValueRangeLengthZ );
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
                trackSettings.m_scaleRangeX = { rawScaleValueRangeX.m_start, Math::IsNearZero( rawScaleValueRangeLengthX ) ? defaultQuantizationRangeLength : rawScaleValueRangeLengthX };
                trackSettings.m_scaleRangeY = { rawScaleValueRangeY.m_start, Math::IsNearZero( rawScaleValueRangeLengthY ) ? defaultQuantizationRangeLength : rawScaleValueRangeLengthY };
                trackSettings.m_scaleRangeZ = { rawScaleValueRangeZ.m_start, Math::IsNearZero( rawScaleValueRangeLengthZ ) ? defaultQuantizationRangeLength : rawScaleValueRangeLengthZ };
            }

            //-------------------------------------------------------------------------

            if ( trackSettings.IsScaleTrackStatic() )
            {
                Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[0];
                Vector const& scale = rawBoneTransform.GetScale();

                uint16 const m_x = Quantization::EncodeFloat( scale.m_x, trackSettings.m_scaleRangeX.m_rangeStart, trackSettings.m_scaleRangeX.m_rangeLength );
                uint16 const m_y = Quantization::EncodeFloat( scale.m_y, trackSettings.m_scaleRangeY.m_rangeStart, trackSettings.m_scaleRangeY.m_rangeLength );
                uint16 const m_z = Quantization::EncodeFloat( scale.m_z, trackSettings.m_scaleRangeZ.m_rangeStart, trackSettings.m_scaleRangeZ.m_rangeLength );

                animClip.m_compressedPoseData.push_back( m_x );
                animClip.m_compressedPoseData.push_back( m_y );
                animClip.m_compressedPoseData.push_back( m_z );
            }
            else // Store all frames
            {
                for ( uint32 frameIdx = 0; frameIdx < animClip.m_numFrames; frameIdx++ )
                {
                    Transform const& rawBoneTransform = rawTrackData[boneIdx].m_transforms[frameIdx];
                    Vector const& scale = rawBoneTransform.GetScale();

                    uint16 const m_x = Quantization::EncodeFloat( scale.m_x, trackSettings.m_scaleRangeX.m_rangeStart, trackSettings.m_scaleRangeX.m_rangeLength );
                    uint16 const m_y = Quantization::EncodeFloat( scale.m_y, trackSettings.m_scaleRangeY.m_rangeStart, trackSettings.m_scaleRangeY.m_rangeLength );
                    uint16 const m_z = Quantization::EncodeFloat( scale.m_z, trackSettings.m_scaleRangeZ.m_rangeStart, trackSettings.m_scaleRangeZ.m_rangeLength );

                    animClip.m_compressedPoseData.push_back( m_x );
                    animClip.m_compressedPoseData.push_back( m_y );
                    animClip.m_compressedPoseData.push_back( m_z );
                }
            }

            //-------------------------------------------------------------------------

            animClip.m_trackCompressionSettings.emplace_back( trackSettings );
        }
    }

    //-------------------------------------------------------------------------

    bool AnimationClipCompiler::ReadEventsData( Resource::CompileContext const& ctx, rapidjson::Document const& document, RawAssets::RawAnimation const& rawAnimData, AnimationEventData& outEventData ) const
    {
        KRG_ASSERT( document.IsObject() );

        // Read event track data
        //-------------------------------------------------------------------------

        // Check if there is potential additional event data
        auto trackDataIter = document.FindMember( EventTrack::s_eventTrackContainerKey );
        if ( trackDataIter == document.MemberEnd() )
        {
            return true;
        }

        auto const& eventDataValueObject = trackDataIter->value;
        if ( !eventDataValueObject.IsArray() )
        {
            Error( "Malformed event track data" );
            return false;
        }

        Timeline::TrackContainer trackContainer;
        if ( !trackContainer.Load( ctx.m_typeRegistry, eventDataValueObject ) )
        {
            Error( "Malformed event track data" );
            return false;
        }

        // Reflect into runtime events
        //-------------------------------------------------------------------------

        int32 numSyncTracks = 0;
        TVector<Event*> events;
        FloatRange const animationTimeRange( 0, rawAnimData.GetDuration() );
        for ( auto pTrack : trackContainer.m_tracks )
        {
            auto pEventTrack = Cast<EventTrack>( pTrack );

            if ( pEventTrack->IsSyncTrack() )
            {
                numSyncTracks++;
            }

            for ( auto const pItem : pTrack->GetItems() )
            {
                auto pEvent = Cast<EventItem>( pItem )->GetEvent();

                // Add event
                //-------------------------------------------------------------------------

                if ( !animationTimeRange.ContainsInclusive( pEvent->GetTimeRange() ) )
                {
                    Warning( "Event detected outside animation time range, event will be ignored" );
                    continue;
                }

                // TODO: Clamp event to animation length
                events.emplace_back( pEvent );

                // Create sync event
                //-------------------------------------------------------------------------

                if ( pEventTrack->IsSyncTrack() && numSyncTracks == 1 )
                {
                    outEventData.m_syncEventMarkers.emplace_back( SyncTrack::EventMarker( Percentage( pEvent->GetStartTime() / rawAnimData.GetDuration() ), pEvent->GetSyncEventID() ) );
                }
            }
        }

        if ( numSyncTracks > 1 )
        {
            Warning( "Multiple sync tracks detected, using the first one encountered!" );
        }

        // Transfer sorted events
        //-------------------------------------------------------------------------

        auto sortPredicate = [] ( Event* const& pEventA, Event* const& pEventB )
        {
            return pEventA->GetStartTime() < pEventB->GetStartTime();
        };

        eastl::sort( events.begin(), events.end(), sortPredicate );

        for ( auto const& pEvent : events )
        {
            outEventData.m_collection.m_descriptors.emplace_back( TypeSystem::TypeDescriptor( ctx.m_typeRegistry, pEvent ) );
        }

        eastl::sort( outEventData.m_syncEventMarkers.begin(), outEventData.m_syncEventMarkers.end() );

        // Free allocated memory
        //-------------------------------------------------------------------------

        trackContainer.Reset();

        return true;
    }
}