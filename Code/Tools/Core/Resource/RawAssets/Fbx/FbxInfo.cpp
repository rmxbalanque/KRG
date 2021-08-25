#include "FbxInfo.h"
#include "FbxSceneContext.h"
#include "Tools/Core/Resource/RawAssets/RawAssetInfo.h"

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    static void ReadMeshInfo( FbxSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        FbxGeometryConverter geomConverter( sceneCtx.m_pManager );
        geomConverter.SplitMeshesPerMaterial( sceneCtx.m_pScene, true );

        auto numGeometries = sceneCtx.m_pScene->GetGeometryCount();
        for ( auto i = 0; i < numGeometries; i++ )
        {
            auto pGeometry = sceneCtx.m_pScene->GetGeometry( i );
            if ( pGeometry->Is<FbxMesh>() )
            {
                FbxMesh* pMesh = static_cast<FbxMesh*>( pGeometry );

                RawAssets::RawMeshInfo meshInfo;
                meshInfo.m_name = pMesh->GetNode()->GetName();
                meshInfo.m_isSkinned = pMesh->GetDeformerCount( FbxDeformer::eSkin ) > 0;
                //meshInfo.m_materialName = ;
                outInfo.m_meshes.emplace_back( meshInfo );
            }
        }
    }

    static void ReadAnimationInfo( FbxSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        auto const numAnimStacks = sceneCtx.m_pScene->GetSrcObjectCount<FbxAnimStack>();
        for ( auto i = 0; i < numAnimStacks; i++ )
        {
            auto pAnimStack = sceneCtx.m_pScene->GetSrcObject<FbxAnimStack>( i );

            RawAssets::RawAnimationInfo animInfo;
            animInfo.m_name = pAnimStack->GetName();

            auto pTakeInfo = sceneCtx.m_pScene->GetTakeInfo( pAnimStack->GetNameWithoutNameSpacePrefix() );
            if ( pTakeInfo != nullptr )
            {
                animInfo.m_duration = (float) pTakeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble();

                FbxTime const duration = pTakeInfo->mLocalTimeSpan.GetDuration();
                FbxTime::EMode mode = duration.GetGlobalTimeMode();
                animInfo.m_frameRate = (float) duration.GetFrameRate( mode );
            }

            outInfo.m_animations.emplace_back( animInfo );
        }
    }

    static void ReadSkeletonInfo( FbxSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        TVector<FbxNode*> skeletonRootNodes;
        sceneCtx.FindAllNodesOfType( FbxNodeAttribute::eSkeleton, skeletonRootNodes );

        for ( auto& pSkeletonNode : skeletonRootNodes )
        {
            RawAssets::RawSkeletonInfo skeletonInfo;
            skeletonInfo.m_name = pSkeletonNode->GetName();
            outInfo.m_skeletons.emplace_back( skeletonInfo );
        }
    }

    bool ReadFileInfo( FileSystem::Path const& sourceFilePath, RawAssets::RawAssetInfo& outInfo )
    {
        FbxSceneContext sceneCtx( sourceFilePath );
        if ( !sceneCtx.IsValid() )
        {
            return false;
        }

        outInfo.m_upAxis = sceneCtx.GetOriginalUpAxis();
        outInfo.m_scale = sceneCtx.GetScaleConversionMultiplier();

        //-------------------------------------------------------------------------

        ReadMeshInfo( sceneCtx, outInfo );
        ReadAnimationInfo( sceneCtx, outInfo );
        ReadSkeletonInfo( sceneCtx, outInfo );

        return true;
    }
}