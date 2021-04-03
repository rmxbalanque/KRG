#ifdef _WIN32
#include "SkeletalMeshCompiler.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Resource/RawAssets/RawMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "MeshOptimizer.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    SkeletalMeshCompiler::SkeletalMeshCompiler()
        : MeshCompiler( "SkeletalMeshCompiler", VERSION )
    {
        m_outputTypes.push_back( SkeletalMesh::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult SkeletalMeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        SkeletalMeshResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        bool const isSkeletalMesh = ( ctx.m_resourceID.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID() );

        // Read mesh data
        //-------------------------------------------------------------------------

        FileSystem::Path meshFilePath;
        if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_meshDataPath, meshFilePath ) )
        {
            return Error( "Invalid mesh data path: %s", resourceDescriptor.m_meshDataPath.c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        TUniquePtr<RawAssets::RawMesh> pRawMesh = RawAssets::ReadSkeletalMesh( readerCtx, meshFilePath, 4 );
        if ( pRawMesh == nullptr )
        {
            return Error( "Failed to read mesh from source file" );
        }

        KRG_ASSERT( pRawMesh->IsValid() );

        // Reflect FBX data into runtime format
        //-------------------------------------------------------------------------

        SkeletalMesh skeletalMesh;
        TransferMeshGeometry( *pRawMesh, skeletalMesh );
        OptimizeMeshGeometry( skeletalMesh );
        TransferSkeletalMeshData( *pRawMesh, skeletalMesh );
        SetMeshDefaultMaterials( resourceDescriptor, skeletalMesh );

        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( VERSION, resourceDescriptor.m_resourceTypeID );

            SetMeshInstallDependencies( skeletalMesh, hdr );

            archive << hdr << skeletalMesh;

            if ( pRawMesh->HasWarnings() )
            {
                return CompilationSucceededWithWarnings( ctx );
            }
            else
            {
                return CompilationSucceeded( ctx );
            }
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    void SkeletalMeshCompiler::TransferSkeletalMeshData( RawAssets::RawMesh const& rawMesh, SkeletalMesh& mesh ) const
    {
        KRG_ASSERT( rawMesh.IsSkeletalMesh() );

        auto const& skeleton = rawMesh.GetSkeleton();
        auto const& boneData = skeleton.GetBoneData();

        auto const numBones = skeleton.GetNumBones();
        for ( auto i = 0; i < numBones; i++ )
        {
            mesh.m_boneIDs.push_back( boneData[i].m_name );
            mesh.m_parentBoneIndices.push_back( boneData[i].m_parentBoneIdx );
            mesh.m_bindPose.push_back( boneData[i].m_globalTransform );
        }
    }
}
#endif