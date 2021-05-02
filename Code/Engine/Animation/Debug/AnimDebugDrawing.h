#pragma once

#include "../_Module/API.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_ENGINE_ANIMATION_API void DrawRootMotionGizmo( Debug::DrawingContext& drawingContext, Transform const& transform, Color color );
}