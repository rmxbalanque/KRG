#pragma once

#include "../_Module/API.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Skeleton;
        class Pose;

        KRG_ENGINE_ANIMATION_API void DrawSkeleton( Debug::DrawingContext& drawingContext, Skeleton const& skeleton, Transform const& worldTransform );
        KRG_ENGINE_ANIMATION_API void DrawPose( Debug::DrawingContext& drawingContext, Pose const* pPose, Transform const& worldTransform );
        KRG_ENGINE_ANIMATION_API void DrawGlobalPose( Debug::DrawingContext& drawingContext, Pose const* pPose, Transform const& worldTransform );
        KRG_ENGINE_ANIMATION_API void DrawRootMotionGizmo( Debug::DrawingContext& drawingContext, Transform const& transform, Color color );
        KRG_ENGINE_ANIMATION_API void DrawRootMotionPath( Debug::DrawingContext& drawingContext, TVector<Transform> const& rootMotionTransforms, Transform const& worldTransform, Color const& color0, Color const& color1, F32 axisSize = 0.25f );
    }
}