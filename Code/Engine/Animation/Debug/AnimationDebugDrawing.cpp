#include "AnimationDebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void DrawRootMotionGizmo( Debug::DrawingContext& drawingContext, Transform const& transform, Color color )
    {
        static float const gizmoRadius = 0.35f;
        static float const arrowGap = 0.01f;
        Transform const rootMotionMatrix = transform;

        Vector fwdDir = rootMotionMatrix.GetAxisY();
        Vector arrowStartPos = Vector::MultiplyAdd( fwdDir, Vector( gizmoRadius + arrowGap ), rootMotionMatrix.GetTranslation() );

        drawingContext.DrawDisc( rootMotionMatrix.GetTranslation(), gizmoRadius, color );
        drawingContext.DrawArrow( arrowStartPos, fwdDir, gizmoRadius, color );
    }
}