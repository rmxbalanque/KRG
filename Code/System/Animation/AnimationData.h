#pragma once

#include "_Module/API.h"
#include "AnimationSkeleton.h"
#include "AnimationFrameTime.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Algorithm/Quantization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Pose;

        //-------------------------------------------------------------------------

        struct QuantizationRange
        {
            KRG_SERIALIZE_MEMBERS( m_rangeStart, m_rangeLength );

            QuantizationRange() = default;

            QuantizationRange( F32 start, F32 length )
                : m_rangeStart( start )
                , m_rangeLength( length )
            {}

            inline bool IsValid() const { return m_rangeLength > 0; }

        public:

            F32                                     m_rangeStart = 0;
            F32                                     m_rangeLength = -1;
        };

        //-------------------------------------------------------------------------

        struct TrackCompressionSettings
        {
            KRG_SERIALIZE_MEMBERS( m_translationRangeX, m_translationRangeY, m_translationRangeZ, m_scaleRange, m_trackStartIndex, m_isTranslationStatic, m_isScaleStatic );

            friend class AnimationCompiler;

        public:

            TrackCompressionSettings() = default;

            // Is the translation for this track static i.e. a fixed value for the duration of the animation
            inline bool IsTranslationTrackStatic() const { return m_isTranslationStatic; }

            // Is the scale for this track static i.e. a fixed value for the duration of the animation
            inline bool IsScaleTrackStatic() const { return m_isScaleStatic; }

        public:

            QuantizationRange                       m_translationRangeX;
            QuantizationRange                       m_translationRangeY;
            QuantizationRange                       m_translationRangeZ;
            QuantizationRange                       m_scaleRange;
            U32                                     m_trackStartIndex = 0; // The start offset for this track in the compressed data block (in number of U16s)

        private:

            bool                                    m_isTranslationStatic = false;
            bool                                    m_isScaleStatic = false;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_ANIMATION_API AnimationData : public Resource::IResource
        {
            KRG_REGISTER_RESOURCE( 'ANIM' );
            KRG_SERIALIZE_MEMBERS( m_pSkeleton, m_duration, m_numFrames, m_compressedPoseData, m_trackCompressionSettings, m_isAdditive );

            friend class AnimationCompiler;
            friend class AnimationLoader;

        private:

            inline static Quaternion DecodeRotation( U16 const* pData );
            inline static Vector DecodeTranslation( U16 const* pData, TrackCompressionSettings const& settings );
            inline static F32 DecodeScale( U16 const* pData, TrackCompressionSettings const& settings );

        public:

            AnimationData() = default;

            virtual bool IsValid() const final { return m_pSkeleton != nullptr && m_pSkeleton.IsLoaded() && m_numFrames > 0; }
            inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }
            inline S32 GetNumBones() const { KRG_ASSERT( m_pSkeleton != nullptr ); return m_pSkeleton->GetNumBones(); }

            // Animation Info
            //-------------------------------------------------------------------------

            inline bool IsSingleFrameAnimation() const { return m_numFrames == 1; }
            inline bool IsAdditive() const { return m_isAdditive; }
            inline F32 GetFPS() const { return ((F32) m_numFrames ) / m_duration; }
            inline U32 GetNumberOfFrames() const { return m_numFrames; }
            inline Seconds GetDuration() const { return m_duration; }
            inline Seconds GetTime( U32 frame ) const { return Seconds( GetPercentageThrough( frame ).ToFloat() * m_duration ); }
            inline Percentage GetPercentageThrough( U32 frame ) const { return Percentage( ( (F32) frame ) / m_numFrames ); }
            FrameTime GetFrameTime( Percentage const percentageThrough ) const;

            // Sampling
            //-------------------------------------------------------------------------

            void GetPose( FrameTime const& frameTime, Pose* pOutPose ) const;
            inline void GetPose( Percentage percentageThrough, Pose* pOutPose ) const { GetPose( GetFrameTime( percentageThrough ), pOutPose ); }

            Transform GetLocalSpaceTransform( S32 boneIdx, FrameTime const& frameTime ) const;
            inline Transform GetLocalSpaceTransform( S32 boneIdx, Percentage percentageThrough ) const{ return GetLocalSpaceTransform( boneIdx, GetFrameTime( percentageThrough ) ); }

            // Warning: these are expensive functions, use sparingly
            Transform GetGlobalSpaceTransform( S32 boneIdx, FrameTime const& frameTime ) const;
            inline Transform GetGlobalSpaceTransform( S32 boneIdx, Percentage percentageThrough ) const{ return GetGlobalSpaceTransform( boneIdx, GetFrameTime( percentageThrough ) ); }

            Transform GetDisplacementTransform( FrameTime const& frameTime ) const;
            inline Transform GetDisplacementTransform( Percentage percentageThrough ) const { return GetDisplacementTransform( GetFrameTime( percentageThrough ) ); }
            inline Transform GetDisplacementDelta( TRange<Percentage> const& timeRange ) const;

        private:

            // Read a compressed transform from a track and return a pointer to the data for the next track
            inline U16 const* ReadCompressedTrackTransform( U16 const* pTrackData, TrackCompressionSettings const& trackSettings, FrameTime const& frameTime, Transform& outTransform ) const;
            inline U16 const* ReadCompressedTrackKeyFrame( U16 const* pTrackData, TrackCompressionSettings const& trackSettings, U32 frameIdx, Transform& outTransform ) const;

        private:

            TResourcePtr<Skeleton>                  m_pSkeleton;
            U32                                     m_numFrames = 0;
            Seconds                                 m_duration = 0.0f;
            TVector<U16>                            m_compressedPoseData;
            TVector<TrackCompressionSettings>       m_trackCompressionSettings;
            TVector<Transform>                      m_displacementTrack;
            bool                                    m_isAdditive = false;
        };

        //-------------------------------------------------------------------------

        inline Quaternion AnimationData::DecodeRotation( U16 const* pData )
        {
            Quantization::EncodedQuaternion const encodedQuat( pData[0], pData[1], pData[2] );
            return encodedQuat.ToQuaternion();
        }

        inline Vector AnimationData::DecodeTranslation( U16 const* pData, TrackCompressionSettings const& settings )
        {
            F32 const x = Quantization::DecodeFloat( pData[0], settings.m_translationRangeX.m_rangeStart, settings.m_translationRangeX.m_rangeLength );
            F32 const y = Quantization::DecodeFloat( pData[1], settings.m_translationRangeY.m_rangeStart, settings.m_translationRangeY.m_rangeLength );
            F32 const z = Quantization::DecodeFloat( pData[2], settings.m_translationRangeZ.m_rangeStart, settings.m_translationRangeZ.m_rangeLength );
            return Vector( x, y, z );
        }

        inline F32 AnimationData::DecodeScale( U16 const* pData, TrackCompressionSettings const& settings )
        {
            return Quantization::DecodeFloat( *pData, settings.m_scaleRange.m_rangeStart, settings.m_scaleRange.m_rangeLength );
        }

        inline Transform AnimationData::GetDisplacementDelta( TRange<Percentage> const& timeRange ) const
        {
            Transform const startTransform = GetDisplacementTransform( timeRange.m_min );
            Transform const endTransform = GetDisplacementTransform( timeRange.m_max );
            return Transform::DeltaNoScale( startTransform, endTransform );
        }

        //-------------------------------------------------------------------------

        // This is in the header so it will be inlined - Do not move to the CPP file
        inline U16 const* AnimationData::ReadCompressedTrackTransform( U16 const* pTrackData, TrackCompressionSettings const& trackSettings, FrameTime const& frameTime, Transform& outTransform ) const
        {
            KRG_ASSERT( pTrackData != nullptr );

            U32 const frameIdx = frameTime.GetFrameIndex();
            Percentage const percentageThrough = frameTime.GetPercentageThrough();
            KRG_ASSERT( frameIdx < m_numFrames - 1 );

            //-------------------------------------------------------------------------

            Transform transform0;
            Transform transform1;

            //-------------------------------------------------------------------------
            // Read rotation
            //-------------------------------------------------------------------------

            // Rotations are 48bits (3 x U16)
            static constexpr U32 const rotationStride = 3;

            U32 PoseDataIdx0 = frameIdx * rotationStride;
            U32 PoseDataIdx1 = PoseDataIdx0 + rotationStride;
            KRG_ASSERT( PoseDataIdx1 < ( m_numFrames * rotationStride ) );

            transform0.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx0] ) );
            transform1.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx1] ) );

            // Shift the track data ptr to the translation data
            pTrackData += ( m_numFrames * rotationStride );

            //-------------------------------------------------------------------------
            // Read translation
            //-------------------------------------------------------------------------

            // Translations are 48bits (3 x U16)
            static constexpr U32 const translationStride = 3;

            if ( trackSettings.IsTranslationTrackStatic() )
            {
                Vector const translation = DecodeTranslation( pTrackData, trackSettings );
                transform0.SetTranslation( translation );
                transform1.SetTranslation( translation );

                // Shift the track data ptr to the scale data
                pTrackData += translationStride;
            }
            else // Read the translation key-frames
            {
                PoseDataIdx0 = frameIdx * translationStride;
                PoseDataIdx1 = PoseDataIdx0 + translationStride;
                KRG_ASSERT( PoseDataIdx1 < ( m_numFrames * translationStride ) );

                transform0.SetTranslation( DecodeTranslation( &pTrackData[PoseDataIdx0], trackSettings ) );
                transform1.SetTranslation( DecodeTranslation( &pTrackData[PoseDataIdx1], trackSettings ) );

                // Shift the track data ptr to the translation data
                pTrackData += ( m_numFrames * rotationStride );
            }

            //-------------------------------------------------------------------------
            // Read scale
            //-------------------------------------------------------------------------

            // Scales are 16bits (1 x U16)
            static constexpr U32 const scaleStride = 1;

            if ( trackSettings.IsScaleTrackStatic() )
            {
                F32 const scale = DecodeScale( pTrackData, trackSettings );
                transform0.SetScale( scale );
                transform1.SetScale( scale );

                // Shift the track data ptr to the next track's rotation data
                pTrackData += scaleStride;
            }
            else // Read the scale key-frames
            {
                PoseDataIdx0 = frameIdx * scaleStride;
                PoseDataIdx1 = PoseDataIdx0 + scaleStride;
                KRG_ASSERT( PoseDataIdx1 < ( m_numFrames * scaleStride ) );

                transform0.SetScale( DecodeScale( &pTrackData[PoseDataIdx0], trackSettings ) );
                transform1.SetScale( DecodeScale( &pTrackData[PoseDataIdx1], trackSettings ) );

                // Shift the track data ptr to the next track's rotation data
                pTrackData += ( m_numFrames * scaleStride );
            }

            //-------------------------------------------------------------------------

            outTransform = Transform::Slerp( transform0, transform1, percentageThrough );

            //-------------------------------------------------------------------------

            return pTrackData;
        }

        // This is in the header so it will be inlined - Do not move to the CPP file
        inline U16 const* AnimationData::ReadCompressedTrackKeyFrame( U16 const* pTrackData, TrackCompressionSettings const& trackSettings, U32 frameIdx, Transform& outTransform ) const
        {
            KRG_ASSERT( pTrackData != nullptr );
            KRG_ASSERT( frameIdx < m_numFrames );

            //-------------------------------------------------------------------------
            // Read rotation
            //-------------------------------------------------------------------------

            // Rotations are 48bits (3 x U16)
            static constexpr U32 const rotationStride = 3;

            U32 PoseDataIdx = frameIdx * rotationStride;
            KRG_ASSERT( PoseDataIdx < ( m_numFrames * rotationStride ) );

            outTransform.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx] ) );

            // Shift the track data ptr to the translation data
            pTrackData += ( m_numFrames * rotationStride );

            //-------------------------------------------------------------------------
            // Read translation
            //-------------------------------------------------------------------------

            // Translations are 48bits (3 x U16)
            static constexpr U32 const translationStride = 3;

            if ( trackSettings.IsTranslationTrackStatic() )
            {
                Vector const translation = DecodeTranslation( pTrackData, trackSettings );
                outTransform.SetTranslation( translation );

                // Shift the track data ptr to the scale data
                pTrackData += translationStride;
            }
            else // Read the translation key-frames
            {
                PoseDataIdx = frameIdx * translationStride;
                KRG_ASSERT( PoseDataIdx < ( m_numFrames * translationStride ) );

                outTransform.SetTranslation( DecodeTranslation( &pTrackData[PoseDataIdx], trackSettings ) );

                // Shift the track data ptr to the translation data
                pTrackData += ( m_numFrames * rotationStride );
            }

            //-------------------------------------------------------------------------
            // Read scale
            //-------------------------------------------------------------------------

            // Scales are 16bits (1 x U16)
            static constexpr U32 const scaleStride = 1;

            if ( trackSettings.IsScaleTrackStatic() )
            {
                F32 const scale = DecodeScale( pTrackData, trackSettings );
                outTransform.SetScale( scale );

                // Shift the track data ptr to the next track's rotation data
                pTrackData += scaleStride;
            }
            else // Read the scale key-frames
            {
                PoseDataIdx = frameIdx * scaleStride;
                KRG_ASSERT( PoseDataIdx < ( m_numFrames * scaleStride ) );

                outTransform.SetScale( DecodeScale( &pTrackData[PoseDataIdx], trackSettings ) );

                // Shift the track data ptr to the next track's rotation data
                pTrackData += ( m_numFrames * scaleStride );
            }

            //-------------------------------------------------------------------------

            return pTrackData;
        }
    }
}