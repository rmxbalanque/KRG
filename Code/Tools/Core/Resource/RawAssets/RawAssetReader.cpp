#include "RawAssetReader.h"
#include "RawAssetInfo.h"
#include "Fbx/FbxSkeleton.h"
#include "Fbx/FbxAnimation.h"
#include "Fbx/FbxMesh.h"
#include "Fbx/FbxInfo.h"
#include "gltf/gltfMesh.h"
#include "gltf/gltfSkeleton.h"
#include "gltf/gltfAnimation.h"
#include "gltf/gltfInfo.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    static bool ValidateRawAsset( ReaderContext const& ctx, RawAssets::RawAsset const* pRawAsset )
    {
        if ( pRawAsset )
        {
            for ( auto const& warning : pRawAsset->GetWarnings() )
            {
                ctx.m_warningDelegate( warning.c_str() );
            }

            for ( auto const& error : pRawAsset->GetErrors() )
            {
                ctx.m_errorDelegate( error.c_str() );
            }

            return !pRawAsset->HasErrors() && pRawAsset->IsValid();
        }

        return false;
    }

    //-------------------------------------------------------------------------

    bool ReadFileInfo( FileSystem::Path const& sourceFilePath, RawAssetInfo& outInfo )
    {
        KRG_ASSERT( sourceFilePath.IsValid() );

        outInfo.Reset();

        //-------------------------------------------------------------------------

        auto const extension = sourceFilePath.GetExtensionAsString();
        if ( extension == "fbx" )
        {
            return Fbx::ReadFileInfo( sourceFilePath, outInfo );
        }
        else if ( extension == "gltf" || extension == "glb" )
        {
            return gltf::ReadFileInfo( sourceFilePath, outInfo );
        }
        else
        {
            KRG_LOG_ERROR( "Raw Assets", "Unsupported extension: %s", sourceFilePath.c_str() );
        }

        return false;
    }

    //-------------------------------------------------------------------------

    TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( ReaderContext const& ctx, FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile )
    {
        KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

        TUniquePtr<RawAssets::RawMesh> pRawMesh = nullptr;

        auto const extension = sourceFilePath.GetExtensionAsString();
        if ( extension == "fbx" )
        {
            pRawMesh = Fbx::ReadStaticMesh( sourceFilePath, nameOfMeshToCompile );
        }
        else if ( extension == "gltf" || extension == "glb" )
        {
            pRawMesh = gltf::ReadStaticMesh( sourceFilePath, nameOfMeshToCompile );
        }
        else
        {
            char buffer[512];
            Printf( buffer, 512, "unsupported extension: %s", sourceFilePath.c_str() );
            ctx.m_errorDelegate( buffer );
        }

        //-------------------------------------------------------------------------

        if ( !ValidateRawAsset( ctx, pRawMesh.get() ) )
        {
            pRawMesh = nullptr;
        }

        //-------------------------------------------------------------------------

        return pRawMesh;
    }

    TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( ReaderContext const& ctx, FileSystem::Path const& sourceFilePath, int32 maxBoneInfluences )
    {
        KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

        TUniquePtr<RawAssets::RawMesh> pRawMesh = nullptr;

        auto const extension = sourceFilePath.GetExtensionAsString();
        if ( extension == "fbx" )
        {
            pRawMesh = Fbx::ReadSkeletalMesh( sourceFilePath, maxBoneInfluences );
        }
        else if ( extension == "gltf" || extension == "glb" )
        {
            pRawMesh = gltf::ReadSkeletalMesh( sourceFilePath, maxBoneInfluences );
        }
        else
        {
            char buffer[512];
            Printf( buffer, 512, "unsupported extension: %s", sourceFilePath.c_str() );
            ctx.m_errorDelegate( buffer );
        }

        //-------------------------------------------------------------------------

        if ( !ValidateRawAsset( ctx, pRawMesh.get() ) )
        {
            pRawMesh = nullptr;
        }

        //-------------------------------------------------------------------------

        return pRawMesh;
    }

    TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( ReaderContext const& ctx, FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName )
    {
        KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

        TUniquePtr<RawAssets::RawSkeleton> pRawSkeleton = nullptr;

        auto const extension = sourceFilePath.GetExtensionAsString();
        if ( extension == "fbx" )
        {
            pRawSkeleton = Fbx::ReadSkeleton( sourceFilePath, skeletonRootBoneName );
        }
        else if ( extension == "gltf" || extension == "glb" )
        {
            pRawSkeleton = gltf::ReadSkeleton( sourceFilePath, skeletonRootBoneName );
        }
        else
        {
            char buffer[512];
            Printf( buffer, 512, "unsupported extension: %s", sourceFilePath.c_str() );
            ctx.m_errorDelegate( buffer );
        }

        //-------------------------------------------------------------------------

        if ( !ValidateRawAsset( ctx, pRawSkeleton.get() ) )
        {
            pRawSkeleton = nullptr;
        }

        //-------------------------------------------------------------------------

        return pRawSkeleton;
    }

    TUniquePtr<RawAssets::RawAnimation> ReadAnimation( ReaderContext const& ctx, FileSystem::Path const& sourceFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& animationName )
    {
        KRG_ASSERT( ctx.IsValid() && sourceFilePath.IsValid() && rawSkeleton.IsValid() );

        TUniquePtr<RawAssets::RawAnimation> pRawAnimation = nullptr;

        auto const extension = sourceFilePath.GetExtensionAsString();
        if ( extension == "fbx" )
        {
            pRawAnimation = Fbx::ReadAnimation( sourceFilePath, rawSkeleton, animationName );
        }
        else if ( extension == "gltf" || extension == "glb" )
        {
            pRawAnimation = gltf::ReadAnimation( sourceFilePath, rawSkeleton, animationName );
        }
        else
        {
            InlineString<512> errorString;
            errorString.sprintf( "unsupported extension: %s", sourceFilePath.c_str() );
            ctx.m_errorDelegate( errorString.c_str() );
        }

        //-------------------------------------------------------------------------

        pRawAnimation->Finalize();

        //-------------------------------------------------------------------------

        if ( !ValidateRawAsset( ctx, pRawAnimation.get() ) )
        {
            pRawAnimation = nullptr;
        }

        //-------------------------------------------------------------------------

        return pRawAnimation;
    }
}