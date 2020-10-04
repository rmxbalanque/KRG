#ifdef _WIN32

#include "RawAssetReader.h"
#include "RawAnimation.h"
#include "RawMesh.h"
#include "Fbx/FbxSkeleton.h"
#include "Fbx/FbxAnimation.h"
#include "Fbx/FbxMesh.h"
#include "gltf/gltfMesh.h"
#include "gltf/gltfSkeleton.h"
#include "gltf/gltfAnimation.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
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

        TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, String const& nameOfMeshToCompile )
        {
            KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

            TUniquePtr<RawAssets::RawMesh> pRawMesh = nullptr;

            if ( sourceFilePath.GetExtension() == "fbx" )
            {
                pRawMesh = Fbx::ReadStaticMesh( sourceFilePath, nameOfMeshToCompile );
            }
            else if ( sourceFilePath.GetExtension() == "gltf" || sourceFilePath.GetExtension() == "glb" )
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

        TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, S32 maxBoneInfluences )
        {
            KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

            TUniquePtr<RawAssets::RawMesh> pRawMesh = nullptr;

            if ( sourceFilePath.GetExtension() == "fbx" )
            {
                pRawMesh = Fbx::ReadSkeletalMesh( sourceFilePath, maxBoneInfluences );
            }
            else if ( sourceFilePath.GetExtension() == "gltf" || sourceFilePath.GetExtension() == "glb" )
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

        TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, String const& skeletonRootBoneName )
        {
            KRG_ASSERT( sourceFilePath.IsValid() && ctx.IsValid() );

            TUniquePtr<RawAssets::RawSkeleton> pRawSkeleton = nullptr;

            if ( sourceFilePath.GetExtension() == "fbx" )
            {
                pRawSkeleton = Fbx::ReadSkeleton( sourceFilePath, skeletonRootBoneName );
            }
            else if ( sourceFilePath.GetExtension() == "gltf" || sourceFilePath.GetExtension() == "glb" )
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

        TUniquePtr<RawAssets::RawAnimation> ReadAnimation( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& animationName )
        {
            KRG_ASSERT( ctx.IsValid() && sourceFilePath.IsValid() && rawSkeleton.IsValid() );

            TUniquePtr<RawAssets::RawAnimation> pRawAnimation = nullptr;

            if( sourceFilePath.GetExtension() == "fbx" )
            {
                pRawAnimation = Fbx::ReadAnimation( sourceFilePath, rawSkeleton, animationName );
            }
            else if ( sourceFilePath.GetExtension() == "gltf" || sourceFilePath.GetExtension() == "glb" )
            {
                pRawAnimation = gltf::ReadAnimation( sourceFilePath, rawSkeleton, animationName );
            }
            else
            {
                char buffer[512];
                Printf( buffer, 512, "unsupported extension: %s", sourceFilePath.c_str() );
                ctx.m_errorDelegate( buffer );
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
}

#endif