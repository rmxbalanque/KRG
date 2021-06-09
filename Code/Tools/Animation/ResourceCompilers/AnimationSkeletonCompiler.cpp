#include "AnimationSkeletonCompiler.h"
#include "Tools/Core/Resource/RawAssets/RawSkeleton.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    SkeletonCompiler::SkeletonCompiler() 
        : Resource::Compiler( "SkeletonCompiler", s_version )
    {
        m_outputTypes.push_back( Skeleton::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult SkeletonCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        SkeletonResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Read Skeleton Data
        //-------------------------------------------------------------------------
            
        FileSystem::Path skeletonFilePath;
        if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_skeletonDataPath, skeletonFilePath ) )
        {
            return Error( "Invalid skeleton data path: %s", resourceDescriptor.m_skeletonDataPath.c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        TUniquePtr<RawAssets::RawSkeleton> pRawSkeleton = RawAssets::ReadSkeleton( readerCtx, skeletonFilePath, resourceDescriptor.m_skeletonRootBoneName );
        if ( pRawSkeleton == nullptr )
        {
            return Error( "Failed to read skeleton from source file" );
        }

        // Reflect FBX data into runtime format
        //-------------------------------------------------------------------------
        // TODO: properly support animations with a separate root bone

        Skeleton skeleton;

        int32 const numFbxBones = pRawSkeleton->GetNumBones();
        for ( auto fbxBoneIdx = 0; fbxBoneIdx < numFbxBones; fbxBoneIdx++ )
        {
            auto const& boneData = pRawSkeleton->GetBoneData( fbxBoneIdx );
            skeleton.m_boneIDs.push_back( boneData.m_name );
            skeleton.m_parentIndices.push_back( boneData.m_parentBoneIdx );
            skeleton.m_localReferencePose.push_back( Transform( boneData.m_localTransform.GetRotation(), boneData.m_localTransform.GetTranslation() ) );
        }

        // Serialize skeleton
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, Skeleton::GetStaticResourceTypeID() ) << skeleton;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}