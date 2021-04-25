#pragma once

#include "_Module/API.h"
#include "AnimationSkeleton.h"
#include "AnimationFrameTime.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Time/Time.h"
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

            QuantizationRange( float start, float length )
                : m_rangeStart( start )
                , m_rangeLength( length )
            {}

            inline bool IsValid() const { return m_rangeLength > 0; }

        public:

            float                                     m_rangeStart = 0;
            float                                     m_rangeLength = -1;
        };

        //-------------------------------------------------------------------------

        struct TrackCompressionSettings
        {
            KRG_SERIALIZE_MEMBERS( m_translationRangeX, m_translationRangeY, m_translationRangeZ, m_scaleRangeX, m_scaleRangeY, m_scaleRangeZ, m_trackStartIndex, m_isTranslationStatic, m_isScaleStatic );

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
            QuantizationRange                       m_scaleRangeX;
            QuantizationRange                       m_scaleRangeY;
            QuantizationRange                       m_scaleRangeZ;
            uint32                                  m_trackStartIndex = 0; // The start offset for this track in the compressed data block (in number of uint16s)

        private:

            bool                                    m_isTranslationStatic = false;
            bool                                    m_isScaleStatic = false;
        };

        //-------------------------------------------------------------------------

        class KRG_ENGINE_ANIMATION_API AnimationClip : public Resource::IResource
        {
            KRG_REGISTER_RESOURCE( 'ANIM' );
            KRG_SERIALIZE_MEMBERS( m_pSkeleton, m_duration, m_numFrames, m_compressedPoseData, m_trackCompressionSettings, m_isAdditive );

            friend class AnimationCompiler;
            friend class AnimationLoader;

        private:

            inline static Quaternion DecodeRotation( uint16 const* pData );
            inline static Vector DecodeTranslation( uint16 const* pData, TrackCompressionSettings const& settings );
            inline static Vector DecodeScale( uint16 const* pData, TrackCompressionSettings const& settings );

        public:

            AnimationClip() = default;

            virtual bool IsValid() const final { return m_pSkeleton != nullptr && m_pSkeleton.IsLoaded() && m_numFrames > 0; }
            inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }
            inline int32 GetNumBones() const { KRG_ASSERT( m_pSkeleton != nullptr ); return m_pSkeleton->GetNumBones(); }

            // Animation Info
            //-------------------------------------------------------------------------

            inline bool IsSingleFrameAnimation() const { return m_numFrames == 1; }
            inline bool IsAdditive() const { return m_isAdditive; }
            inline float GetFPS() const { return ((float) m_numFrames ) / m_duration; }
            inline uint32 GetNumFrames() const { return m_numFrames; }
            inline Seconds GetDuration() const { return m_duration; }
            inline Seconds GetTime( uint32 frame ) const { return Seconds( GetPercentageThrough( frame ).ToFloat() * m_duration ); }
            inline Percentage GetPercentageThrough( uint32 frame ) const { return Percentage( ( (float) frame ) / m_numFrames ); }
            FrameTime GetFrameTime( Percentage const percentageThrough ) const;

            // Sampling
            //-------------------------------------------------------------------------

            void GetPose( FrameTime const& frameTime, Pose* pOutPose ) const;
            inline void GetPose( Percentage percentageThrough, Pose* pOutPose ) const { GetPose( GetFrameTime( percentageThrough ), pOutPose ); }

            Transform GetLocalSpaceTransform( int32 boneIdx, FrameTime const& frameTime ) const;
            inline Transform GetLocalSpaceTransform( int32 boneIdx, Percentage percentageThrough ) const{ return GetLocalSpaceTransform( boneIdx, GetFrameTime( percentageThrough ) ); }

            // Warning: these are expensive functions, use sparingly
            Transform GetGlobalSpaceTransform( int32 boneIdx, FrameTime const& frameTime ) const;
            inline Transform GetGlobalSpaceTransform( int32 boneIdx, Percentage percentageThrough ) const{ return GetGlobalSpaceTransform( boneIdx, GetFrameTime( percentageThrough ) ); }

            Transform GetDisplacementTransform( FrameTime const& frameTime ) const;
            inline Transform GetDisplacementTransform( Percentage percentageThrough ) const { return GetDisplacementTransform( GetFrameTime( percentageThrough ) ); }
            inline Transform GetDisplacementDelta( TRange<Percentage> const& timeRange ) const;

        private:

            // Read a compressed transform from a track and return a pointer to the data for the next track
            inline uint16 const* ReadCompressedTrackTransform( uint16 const* pTrackData, TrackCompressionSettings const& trackSettings, FrameTime const& frameTime, Transform& outTransform ) const;
            inline uint16 const* ReadCompressedTrackKeyFrame( uint16 const* pTrackData, TrackCompressionSettings const& trackSettings, uint32 frameIdx, Transform& outTransform ) const;

        private:

            TResourcePtr<Skeleton>                  m_pSkeleton;
            uint32                                  m_numFrames = 0;
            Seconds                                 m_duration = 0.0f;
            TVector<uint16>                         m_compressedPoseData;
            TVector<TrackCompressionSettings>       m_trackCompressionSettings;
            TVector<Transform>                      m_displacementTrack;
            bool                                    m_isAdditive = false;
        };

        //-------------------------------------------------------------------------

        inline Quaternion AnimationClip::DecodeRotation( uint16 const* pData )
        {
            Quantization::EncodedQuaternion const encodedQuat( pData[0], pData[1], pData[2] );
            return encodedQuat.ToQuaternion();
        }

        inline Vector AnimationClip::DecodeTranslation( uint16 const* pData, TrackCompressionSettings const& settings )
        {
            float const m_x = Quantization::DecodeFloat( pData[0], settings.m_translationRangeX.m_rangeStart, settings.m_translationRangeX.m_rangeLength );
            float const m_y = Quantization::DecodeFloat( pData[1], settings.m_translationRangeY.m_rangeStart, settings.m_translationRangeY.m_rangeLength );
            float const m_z = Quantization::DecodeFloat( pData[2], settings.m_translationRangeZ.m_rangeStart, settings.m_translationRangeZ.m_rangeLength );
            return Vector( m_x, m_y, m_z );
        }

        inline Vector AnimationClip::DecodeScale( uint16 const* pData, TrackCompressionSettings const& settings )
        {
            float const m_x = Quantization::DecodeFloat( pData[0], settings.m_scaleRangeX.m_rangeStart, settings.m_scaleRangeX.m_rangeLength );
            float const m_y = Quantization::DecodeFloat( pData[1], settings.m_scaleRangeY.m_rangeStart, settings.m_scaleRangeY.m_rangeLength );
            float const m_z = Quantization::DecodeFloat( pData[2], settings.m_scaleRangeZ.m_rangeStart, settings.m_scaleRangeZ.m_rangeLength );
            return Vector( m_x, m_y, m_z );
        }

        inline Transform AnimationClip::GetDisplacementDelta( TRange<Percentage> const& timeRange ) const
        {
            Transform const startTransform = GetDisplacementTransform( timeRange.m_min );
            Transform const endTransform = GetDisplacementTransform( timeRange.m_max );
            return Transform::DeltaNoScale( startTransform, endTransform );
        }

        //-------------------------------------------------------------------------

        // This is in the header so it will be inlined - Do not move to the CPP file
        inline uint16 const* AnimationClip::ReadCompressedTrackTransform( uint16 const* pTrackData, TrackCompressionSettings const& trackSettings, FrameTime const& frameTime, Transform& outTransform ) const
        {
            KRG_ASSERT( pTrackData != nullptr );

            uint32 const frameIdx = frameTime.GetFrameIndex();
            Percentage const percentageThrough = frameTime.GetPercentageThrough();
            KRG_ASSERT( frameIdx < m_numFrames - 1 );

            //-------------------------------------------------------------------------

            Transform transform0;
            Transform transform1;

            //-------------------------------------------------------------------------
            // Read rotation
            //-------------------------------------------------------------------------

            // Rotations are 48bits (3 x uint16)
            static constexpr uint32 const rotationStride = 3;

            uint32 PoseDataIdx0 = frameIdx * rotationStride;
            uint32 PoseDataIdx1 = PoseDataIdx0 + rotationStride;
            KRG_ASSERT( PoseDataIdx1 < ( m_numFrames * rotationStride ) );

            transform0.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx0] ) );
            transform1.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx1] ) );

            // Shift the track data ptr to the translation data
            pTrackData += ( m_numFrames * rotationStride );

            //-------------------------------------------------------------------------
            // Read translation
            //-------------------------------------------------------------------------

            // Translations are 48bits (3 x uint16)
            static constexpr uint32 const translationStride = 3;

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

            // Scales are 48bits (3 x uint16)
            static constexpr uint32 const scaleStride = 3;

            if ( trackSettings.IsScaleTrackStatic() )
            {
                Vector const scale = DecodeScale( pTrackData, trackSettings );
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
        inline uint16 const* AnimationClip::ReadCompressedTrackKeyFrame( uint16 const* pTrackData, TrackCompressionSettings const& trackSettings, uint32 frameIdx, Transform& outTransform ) const
        {
            KRG_ASSERT( pTrackData != nullptr );
            KRG_ASSERT( frameIdx < m_numFrames );

            //-------------------------------------------------------------------------
            // Read rotation
            //-------------------------------------------------------------------------

            // Rotations are 48bits (3 x uint16)
            static constexpr uint32 const rotationStride = 3;

            uint32 PoseDataIdx = frameIdx * rotationStride;
            KRG_ASSERT( PoseDataIdx < ( m_numFrames * rotationStride ) );

            outTransform.SetRotation( DecodeRotation( &pTrackData[PoseDataIdx] ) );

            // Shift the track data ptr to the translation data
            pTrackData += ( m_numFrames * rotationStride );

            //-------------------------------------------------------------------------
            // Read translation
            //-------------------------------------------------------------------------

            // Translations are 48bits (3 x uint16)
            static constexpr uint32 const translationStride = 3;

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

             // Scales are 48bits (3 x uint16)
            static constexpr uint32 const scaleStride = 3;

            if ( trackSettings.IsScaleTrackStatic() )
            {
                Vector const scale = DecodeScale( pTrackData, trackSettings );
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