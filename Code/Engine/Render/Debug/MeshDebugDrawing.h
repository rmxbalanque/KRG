#pragma once

#include "System/Core/Math/Matrix.h"
#include "System/Core/Debug/DebugDrawing.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace Render
    {
        class SkeletalMesh;

        void DrawBindPose( Debug::DrawingContext& drawingContext, SkeletalMesh const* pMesh, Transform const& worldTransform );
        void DrawBones( Debug::DrawingContext& drawingContext, SkeletalMesh const* pMesh, TVector<Transform> const& boneTransforms, Transform const& worldTransform );
        void DrawNormal( Debug::DrawingContext& drawingContext, SkeletalMesh const* pMesh, Transform const& worldTransform );
    }
}
#endif