#include "AnimationClipCompiler.h"
#include "AnimationSkeletonCompiler.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Resource/RawAssets/RawAnimation.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "Tools/Animation/Events/AnimationEventTrackInfo.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationClipCompiler::AnimationClipCompiler()
        : Resource::Compiler( "AnimationCompiler", VERSION )
    {
        m_outputTypes.push_back( AnimationClip::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationClipCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        AnimationClipResourceDescriptor resourceDescriptor;
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
            return Error( "Invalid skeleton FBX data path: %s", skeletonResourceDescriptor.m_skeletonDataPath.GetString().c_str() );
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

        // Reflect raw animation data into runtime format
        //-------------------------------------------------------------------------

        AnimationClip animData;
        animData.m_pSkeleton = resourceDescriptor.m_pSkeleton;
        TransferAndCompressAnimationData( *pRawAnimation, animData );
        SetRootMotionData( *pRawAnimation, animData );

        // Handle events
        //-------------------------------------------------------------------------

        AnimationEventData eventData;
        if ( !CreateEventsData( ctx, resourceDescriptor, *pRawAnimation, eventData ) )
        {
            return CompilationFailed( ctx );
        }

        // Serialize animation data
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( VERSION, AnimationClip::GetStaticResourceTypeID() );
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

    void AnimationClipCompiler::SetRootMotionData( RawAssets::RawAnimation const& rawAnimData, AnimationClip& animClip ) const
    {
        if ( animClip.GetNumFrames() <= 1 )
        {
            animClip.m_totalRootMotionDelta = Transform::Identity;
            animClip.m_averageLinearVelocity = 0;
            animClip.m_averageAngularVelocity = 0;
            return;
        }

        //-------------------------------------------------------------------------

        float distanceCovered = 0.0f;
        float rotationCovered = 0.0f;

        animClip.m_rootMotionTrack.reserve( animClip.GetNumFrames() );

        for ( auto i = 0u; i < animClip.GetNumFrames(); i++ )
        {
            // TEMP
            static Quaternion const tempRotation( EulerAngles( 0, 0, -180 ) );
            animClip.m_rootMotionTrack.emplace_back( Transform( tempRotation ) );

            // Track deltas
            if ( i > 0 )
            {
                Transform const deltaRoot = Transform::DeltaNoScale( animClip.m_rootMotionTrack[i - 1], animClip.m_rootMotionTrack[i] );
                distanceCovered += deltaRoot.GetTranslation().GetLength3();

                // We use the negative world forward since deltas are relative to the identity transform
                Vector const deltaForward2D = deltaRoot.GetForwardVector().GetNormalized2();
                Radians const deltaAngle = Vector::GetAngleBetweenVectors( deltaForward2D, Vector::WorldBackward ).GetClamped();
                rotationCovered += Math::Abs( deltaAngle );
            }
        }

        animClip.m_totalRootMotionDelta = Transform::DeltaNoScale( animClip.m_rootMotionTrack.front(), animClip.m_rootMotionTrack.back() );
        animClip.m_averageLinearVelocity = distanceCovered / animClip.GetDuration();
        animClip.m_averageAngularVelocity = rotationCovered / animClip.GetDuration();
    }

    //-------------------------------------------------------------------------

    bool AnimationClipCompiler::CreateEventsData( Resource::CompileContext const& ctx, AnimationClipResourceDescriptor const& animResourceDesc, RawAssets::RawAnimation const& rawAnimData, AnimationEventData& outEventData ) const
    {
        // Events are optional
        if ( !animResourceDesc.m_animationEventData.IsValid() )
        {
            return true;
        }

        // If the event data path is set ensure that it exists
        FileSystem::Path const eventFilePath = animResourceDesc.m_animationEventData.ToFileSystemPath( ctx.m_sourceDataPath );
        if ( !eventFilePath.Exists() )
        {
            Error( "Referenced event data file doesnt exist: %s", eventFilePath.c_str() );
            return false;
        }

        // Read json data
        //-------------------------------------------------------------------------

        JsonFileReader trackReader;
        if ( !trackReader.ReadFromFile( eventFilePath ) )
        {
            Error( "Failed to read event track file: %s", eventFilePath.c_str() );
            return false;
        }

        if ( !trackReader.GetDocument().IsArray() )
        {
            Error( "Malformed event track file: %s", eventFilePath.c_str() );
            return false;
        }

        // Read event track data
        //-------------------------------------------------------------------------

        struct EventDescriptor : public TypeSystem::TypeDescriptor
        {
            inline bool operator< ( EventDescriptor const& rhs ) const { return m_startTime < rhs.m_startTime; }
            float m_startTime = 0.0f;
            float m_duration = 0.0f;
        };

        int32 numSyncTracksEncountered = 0;
        TVector<EventDescriptor> events;

        for ( auto& trackObjectValue : trackReader.GetDocument().GetArray() )
        {
            if ( !trackObjectValue.IsObject() )
            {
                Error( "Malformed event track file" );
                return false;
            }

            // Read track info
            //-------------------------------------------------------------------------

            auto trackInfoValueIter = trackObjectValue.FindMember( "TrackInfo" );
            if ( trackInfoValueIter == trackObjectValue.MemberEnd() )
            {
                Error( "Malformed event track file" );
                return false;
            }

            auto const& trackInfoValue = trackInfoValueIter->value;
            if ( !trackInfoValue.IsObject() )
            {
                Error( "Malformed event track file" );
                return false;
            }

            Tools::EventTrackInfo trackInfo;
            if ( !TypeSystem::Serialization::ReadNativeType( ctx.m_typeRegistry, trackInfoValue, &trackInfo ) )
            {
                Error( "Failed to read track info. Malformed event track file" );
                return false;
            }

            if ( trackInfo.m_isSyncTrack )
            {
                numSyncTracksEncountered++;
            }

            // Read events
            //-------------------------------------------------------------------------

            auto eventsArrayIter = trackObjectValue.FindMember( "Events" );
            if ( eventsArrayIter == trackObjectValue.MemberEnd() )
            {
                Error( "Malformed event track file" );
                return false;
            }

            auto const& eventsArray = eventsArrayIter->value;
            if ( !eventsArray.IsArray() )
            {
                Error( "Malformed event track file" );
                return false;
            }

            TRange<float> const animationTimeRange( 0, rawAnimData.GetDuration() );
            for ( auto& eventObjectValue : eventsArray.GetArray() )
            {
                EventDescriptor newEvent;
                if ( !TypeSystem::Serialization::ReadTypeDescriptor( ctx.m_typeRegistry, eventObjectValue, newEvent ) )
                {
                    Error( "Failed reading event descriptor" );
                    return false;
                }

                // Read time info
                //-------------------------------------------------------------------------

                auto startTimeValueIter = eventObjectValue.FindMember( "m_startTime" );
                if ( startTimeValueIter != eventObjectValue.MemberEnd() )
                {
                    if ( !startTimeValueIter->value.IsString() )
                    {
                        Error( "Malformed event detected, start time must be a string" );
                        return false;
                    }

                    TypeSystem::Conversion::ConvertStringToNativeType( ctx.m_typeRegistry, GetCoreTypeID( TypeSystem::CoreTypes::Float ), TypeSystem::TypeID(), startTimeValueIter->value.GetString(), &newEvent.m_startTime );
                }

                auto durationValueIter = eventObjectValue.FindMember( "m_duration" );
                if ( durationValueIter != eventObjectValue.MemberEnd() )
                {
                    if ( !durationValueIter->value.IsString() )
                    {
                        Warning( "Malformed event detected, start time must be a string" );
                        return false;
                    }

                    TypeSystem::Conversion::ConvertStringToNativeType( ctx.m_typeRegistry, GetCoreTypeID( TypeSystem::CoreTypes::Float ), TypeSystem::TypeID(), durationValueIter->value.GetString(), &newEvent.m_duration );
                }

                // Add event
                //-------------------------------------------------------------------------

                TRange<float> const eventRange( newEvent.m_startTime, newEvent.m_duration );
                if ( !animationTimeRange.ContainsInclusive( eventRange ) )
                {
                    Warning( "Event detected outside animation time range, event will be ignored" );
                    continue;
                }

                // TODO: Clamp event to animation length
                events.emplace_back( newEvent );

                // Create sync event
                //-------------------------------------------------------------------------

                if ( trackInfo.m_isSyncTrack && numSyncTracksEncountered == 1 )
                {
                    auto pTypeInfo = ctx.m_typeRegistry.GetTypeInfo( newEvent.m_typeID );
                    KRG_ASSERT( pTypeInfo != nullptr );

                    // Create new event instance to execute the sync ID calculation
                    auto pNewEventInstance = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Event>( ctx.m_typeRegistry, pTypeInfo, newEvent );
                    outEventData.m_syncEventMarkers.emplace_back( SyncTrack::EventMarker( newEvent.m_startTime, pNewEventInstance->GetSyncEventID() ) );
                    KRG::Delete( pNewEventInstance );
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( numSyncTracksEncountered > 1 )
        {
            Warning( "Multiple sync tracks detected, using the first one encountered!" );
        }

        // Transfer sorted events
        //-------------------------------------------------------------------------

        eastl::sort( events.begin(), events.end() );
        for ( auto const& event : events )
        {
            outEventData.m_collection.m_descriptors.emplace_back( event );
        }

        eastl::sort( outEventData.m_syncEventMarkers.begin(), outEventData.m_syncEventMarkers.end() );

        return true;
    }
}