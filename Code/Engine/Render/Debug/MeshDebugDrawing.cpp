#include "MeshDebugDrawing.h"
#include "System/Render/Mesh/SkeletalMesh.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Render
    {
        void DrawBindPose( Debug::DrawingContext& drawingContext, SkeletalMesh const* pMesh, Transform const& worldTransform )
        {
            auto const numBones = pMesh->GetNumBones();
            auto const bindPose = pMesh->GetBindPose();

            Transform boneWorldTransform = bindPose[0] * worldTransform;
            drawingContext.DrawBox( boneWorldTransform, Float3( 0.005f ), Colors::Orange );
            drawingContext.DrawAxis( boneWorldTransform, 0.05f );

            for ( auto i = 1; i < numBones; i++ )
            {
                boneWorldTransform = bindPose[i] * worldTransform;

                auto const parentBoneIdx = pMesh->GetBoneParentIndex( i );
                Transform const parentBoneWorldTransform = bindPose[parentBoneIdx] * worldTransform;

                drawingContext.DrawLine( parentBoneWorldTransform.GetTranslation(), boneWorldTransform.GetTranslation(), Colors::Orange );
                drawingContext.DrawBox( boneWorldTransform, Float3( 0.005f ), Colors::Orange );
                drawingContext.DrawAxis( boneWorldTransform, 0.05f );
            }
        }

        void DrawBones( Debug::DrawingContext& drawingContext, SkeletalMesh const* pMesh, TVector<Transform> const& boneTransforms, Transform const& worldTransform )
        {
            auto const numBones = boneTransforms.size();
            auto const bindPose = pMesh->GetBindPose();

            Transform boneWorldTransform = boneTransforms[0] * worldTransform;
            drawingContext.DrawBox( boneWorldTransform, Float3( 0.005f ), Colors::Orange );
            drawingContext.DrawAxis( boneWorldTransform, 0.05f );

            for ( auto i = 1; i < numBones; i++ )
            {
                boneWorldTransform = boneTransforms[i] * worldTransform;

                auto const parentBoneIdx = pMesh->GetBoneParentIndex( i );
                Transform const parentBoneWorldTransform = boneTransforms[parentBoneIdx] * worldTransform;

                drawingContext.DrawLine( parentBoneWorldTransform.GetTranslation(), boneWorldTransform.GetTranslation(), Colors::Orange );
                //drawingContext.DrawBox( boneWorldTransform, Float3( 0.003f ), Colors::Orange );
                drawingContext.DrawAxis( boneWorldTransform, 0.03f, 2.0f );

                //StringID const& BoneID = pMesh->GetBoneID( i );
                //drawingContext.DrawText3D( boneWorldTransform.GetTranslation(), BoneID.ToString(), Colors::Orange, Debug::Drawing::TextSize::Small );
            }
        }

        void DrawNormal( Debug::DrawingContext& drawingContext, MeshGeometry const* pMesh, Transform const& worldTransform )
        {

        }
    }
}
#endif