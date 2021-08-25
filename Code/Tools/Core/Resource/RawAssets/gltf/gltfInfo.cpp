#include "gltfInfo.h"
#include "gltfSceneContext.h"
#include "Tools/Core/Resource/RawAssets/RawAssetInfo.h"

//-------------------------------------------------------------------------

namespace KRG::gltf
{
    static void ReadMeshInfo( gltfSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        auto pSceneData = sceneCtx.GetSceneData();
        for ( auto m = 0; m < pSceneData->meshes_count; m++ )
        {
            RawAssets::RawMeshInfo meshInfo;
            meshInfo.m_name = pSceneData->meshes[m].name;
            meshInfo.m_isSkinned = pSceneData->meshes[m].weights_count > 0;
            //meshInfo.m_materialName = pSceneData->meshes[m].name;
            outInfo.m_meshes.emplace_back( meshInfo );
        }
    }

    static void ReadAnimationInfo( gltfSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        auto pSceneData = sceneCtx.GetSceneData();
        for ( auto i = 0; i < pSceneData->animations_count; i++ )
        {
            RawAssets::RawAnimationInfo animInfo;
            animInfo.m_name = pSceneData->animations[i].name;

            float animationDuration = -1.0f;
            size_t numFrames = 0;
            for ( auto s = 0; s < pSceneData->animations[i].samplers_count; s++ )
            {
                cgltf_accessor const* pInputAccessor = pSceneData->animations[i].samplers[s].input;
                KRG_ASSERT( pInputAccessor->has_max );
                animationDuration = Math::Max( pInputAccessor->max[0], animationDuration );
                numFrames = Math::Max( pInputAccessor->count, numFrames );
            }

            animInfo.m_duration = animationDuration;
            animInfo.m_frameRate = animationDuration / numFrames;

            outInfo.m_animations.emplace_back( animInfo );
        }
    }

    static void ReadSkeletonInfo( gltfSceneContext const& sceneCtx, RawAssets::RawAssetInfo& outInfo )
    {
        auto pSceneData = sceneCtx.GetSceneData();
        for ( int32 i = 0; i < pSceneData->skins_count; i++ )
        {
            RawAssets::RawSkeletonInfo skeletonInfo;
            skeletonInfo.m_name = pSceneData->skins[i].joints[0]->name;
            outInfo.m_skeletons.emplace_back( skeletonInfo );
        }
    }

    bool ReadFileInfo( FileSystem::Path const& sourceFilePath, RawAssets::RawAssetInfo& outInfo )
    {
        gltfSceneContext sceneCtx( sourceFilePath );
        if ( !sceneCtx.IsValid() )
        {
            return false;
        }

        outInfo.m_upAxis = Axis::Y;

        //-------------------------------------------------------------------------

        ReadMeshInfo( sceneCtx, outInfo );
        ReadAnimationInfo( sceneCtx, outInfo );
        ReadSkeletonInfo( sceneCtx, outInfo );

        return true;
    }
}