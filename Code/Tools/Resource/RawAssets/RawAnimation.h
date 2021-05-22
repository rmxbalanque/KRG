#pragma once

#include "RawSkeleton.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Math/NumericRange.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    class KRG_TOOLS_RESOURCE_API RawAnimation : public RawAsset
    {

    public:

        struct TrackData
        {
            TVector<Transform>                 m_transforms;
            FloatRange                         m_translationValueRangeX;
            FloatRange                         m_translationValueRangeY;
            FloatRange                         m_translationValueRangeZ;
            FloatRange                         m_scaleValueRangeX;
            FloatRange                         m_scaleValueRangeY;
            FloatRange                         m_scaleValueRangeZ;
        };

    public:

        RawAnimation( RawSkeleton const& skeleton ) : m_skeleton( skeleton ) {}
        virtual ~RawAnimation() = default;

        virtual bool IsValid() const override final { return m_numFrames > 0; }

        inline Seconds GetStartTime() const { return m_start; }
        inline Seconds GetEndTime() const { return m_end; }
        inline Seconds GetDuration() const { return m_end - m_start; }
        inline uint32 GetNumFrames() const { return m_numFrames; }
        inline float GetSamplingFrameRate() const { return m_samplingFrameRate; }

        inline int32 GetNumBones() const { return m_skeleton.GetNumBones(); }
        inline TVector<TrackData> const& GetTrackData() const { return m_tracks; }
        inline RawSkeleton const& GetSkeleton() const { return m_skeleton; }

        void Finalize();

    protected:

        RawSkeleton const                   m_skeleton;
        float                               m_samplingFrameRate = 0;
        Seconds                             m_start = 0;
        Seconds                             m_end = 0;
        uint32                              m_numFrames = 0;
        TVector<TrackData>                  m_tracks;
    };
}