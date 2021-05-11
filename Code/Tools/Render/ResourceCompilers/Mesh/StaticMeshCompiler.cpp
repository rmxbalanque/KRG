#include "StaticMeshCompiler.h"
#include "Tools/Resource/RawAssets/RawMesh.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "MeshOptimizer.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    StaticMeshCompiler::StaticMeshCompiler()
        : MeshCompiler( "StaticMeshCompiler", VERSION )
    {
        m_outputTypes.push_back( StaticMesh::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult StaticMeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        StaticMeshResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Read mesh data
        //-------------------------------------------------------------------------

        FileSystem::Path meshFilePath;
        if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_meshDataPath, meshFilePath ) )
        {
            return Error( "Invalid mesh data path: %s", resourceDescriptor.m_meshDataPath.c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        TUniquePtr<RawAssets::RawMesh> pRawMesh = RawAssets::ReadStaticMesh( readerCtx, meshFilePath, resourceDescriptor.m_meshName );
        if ( pRawMesh == nullptr )
        {
            return Error( "Failed to read mesh from source file" );
        }

        KRG_ASSERT( pRawMesh->IsValid() );

        // Reflect FBX data into runtime format
        //-------------------------------------------------------------------------

        StaticMesh staticMesh;
        TransferMeshGeometry( *pRawMesh, staticMesh );
        OptimizeMeshGeometry( staticMesh );
        SetMeshDefaultMaterials( resourceDescriptor, staticMesh );

        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( VERSION, StaticMesh::GetStaticResourceTypeID() );

            SetMeshInstallDependencies( staticMesh, hdr );

            archive << hdr << staticMesh;

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
}