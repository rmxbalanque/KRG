#include "AnimDebugDrawing.h"
#include "Engine/Animation/AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        void DrawLocalTransforms( Debug::DrawingContext& drawingContext, TVector<Transform> const& transforms, TVector<int32> const& parentIndices, Transform const& worldTransform, Color color0, Color color1 )
        {
            auto const numBones = transforms.size();
            if ( numBones > 0 )
            {
                Transform globalTransforms[256];
                globalTransforms[0] = transforms[0] * worldTransform;
                for ( auto i = 1; i < numBones; i++ )
                {
                    auto const& parentIdx = parentIndices[i];
                    auto const& parentTransform = globalTransforms[parentIdx];
                    globalTransforms[i] = transforms[i] * parentTransform;
                }

                for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
                {
                    auto const& parentIdx = parentIndices[boneIdx];
                    auto const& parentTransform = globalTransforms[parentIdx];
                    auto const& boneTransform = globalTransforms[boneIdx];

                    drawingContext.DrawLine( boneTransform.GetTranslation().ToFloat3(), parentTransform.GetTranslation().ToFloat3(), color1, 2.0f );
                    drawingContext.DrawAxis( boneTransform, 0.03f, 3.0f );
                }
            }
        }

        void DrawSkeleton( Debug::DrawingContext& drawingContext, Skeleton const& skeleton, Transform const& worldTransform )
        {
            DrawLocalTransforms( drawingContext, skeleton.GetReferencePose(), skeleton.GetParentIndices(), worldTransform, Colors::HotPink, Colors::LightPink );
        }

        void DrawPose( Debug::DrawingContext& drawingContext, Pose const* pPose, Transform const& worldTransform )
        {
            KRG_ASSERT( pPose != nullptr );
            DrawLocalTransforms( drawingContext, pPose->GetTransforms(), pPose->GetSkeleton()->GetParentIndices(), worldTransform, Colors::HotPink, Colors::LimeGreen );
        }

        void DrawGlobalPose( Debug::DrawingContext& drawingContext, Pose const* pPose, Transform const& worldTransform )
        {
            KRG_ASSERT( pPose != nullptr );
            auto pSkeleton = pPose->GetSkeleton();

            auto rootTransform = pPose->GetGlobalTransform( 0 );
            drawingContext.DrawSphere( rootTransform.GetTranslation().ToFloat3(), Float3( 0.005f ), Colors::HotPink );

            auto const numBones = pPose->GetNumBones();
            for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
            {
                auto const& parentIdx = pSkeleton->GetParentIndex( boneIdx );
                auto const& parentTransform = pPose->GetGlobalTransform( parentIdx );
                auto const& boneTransform = pPose->GetGlobalTransform( boneIdx );

                drawingContext.DrawLine( boneTransform.GetTranslation().ToFloat3(), parentTransform.GetTranslation().ToFloat3(), Colors::Cyan );
                drawingContext.DrawSphere( boneTransform.GetTranslation().ToFloat3(), Float3( 0.005f ), Colors::HotPink );
                drawingContext.DrawAxis( boneTransform, 0.05f );
            }
        }

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

        void DrawRootMotionPath( Debug::DrawingContext& drawingContext, TVector<Transform> const& rootMotionTransforms, Transform const& worldTransform, Color const& color0, Color const& color1, float axisSize /*= 0.25f */ )
        {
            if ( rootMotionTransforms.empty() )
            {
                return;
            }

            auto previousWorldRootMotionTransform = rootMotionTransforms[0] * worldTransform;
            drawingContext.DrawAxis( previousWorldRootMotionTransform, axisSize );

            auto const numTransforms = rootMotionTransforms.size();
            for ( auto i = 1; i < numTransforms; i++ )
            {
                auto const worldRootMotionTransform = rootMotionTransforms[i] * worldTransform;
                drawingContext.DrawLine( previousWorldRootMotionTransform.GetTranslation(), worldRootMotionTransform.GetTranslation(), ( i % 2 == 0 ) ? color0 : color1 );
                drawingContext.DrawAxis( worldRootMotionTransform, axisSize );
                previousWorldRootMotionTransform = worldRootMotionTransform;
            }
        }
    }
}