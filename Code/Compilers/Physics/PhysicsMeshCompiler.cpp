#include "PhysicsMeshCompiler.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Resource/RawAssets/RawMesh.h"
#include "Engine/Physics/Physx.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <cooking/PxCooking.h>

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysxMemoryStream : public PxOutputStream
        {
        public:

            PhysxMemoryStream( TVector<Byte>& destination ) : m_buffer( destination ) {}

        private:

            virtual PxU32 write( const void* src, PxU32 count ) override final
            {
                size_t originalSize = m_buffer.size();
                m_buffer.resize( originalSize + count );
                memcpy( &m_buffer[originalSize], src, count );
                return count;
            }

        private:

            TVector<Byte>& m_buffer;
        };

        //-------------------------------------------------------------------------

        PhysicsMeshCompiler::PhysicsMeshCompiler()
            : Resource::Compiler( "PhysicsGeometryCompiler", VERSION )
        {
            m_outputTypes.push_back('PHYS' );
        }

        Resource::CompilationResult PhysicsMeshCompiler::Compile( Resource::CompileContext const& ctx ) const
        {
            PhysicsMeshResourceDescriptor resourceDescriptor;
            if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
            {
                return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            // Read mesh data
            //-------------------------------------------------------------------------

            FileSystemPath meshFilePath;
            if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_meshDataPath, meshFilePath ) )
            {
                return Error( "Invalid source data path: %s", resourceDescriptor.m_meshDataPath.c_str() );
            }

            RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
            TUniquePtr<RawAssets::RawMesh> pRawMesh = RawAssets::ReadStaticMesh( readerCtx, meshFilePath, resourceDescriptor.m_meshName );
            if ( pRawMesh == nullptr )
            {
                return Error( "Failed to read mesh from source file" );
            }

            KRG_ASSERT( pRawMesh->IsValid() );

            // Reflect FBX data into physics format
            //-------------------------------------------------------------------------
            
            PhysicsMesh physicsGeo;
            TVector<Byte> cookedTriangleMeshData;
            CookTriangleMeshData( *pRawMesh, cookedTriangleMeshData );

            // Serialize
            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                Resource::ResourceHeader hdr( VERSION, resourceDescriptor.m_resourceTypeID );
                archive << hdr << physicsGeo;

                // Serialize the mesh data
                archive << cookedTriangleMeshData;

                if( pRawMesh->HasWarnings() )
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

        bool PhysicsMeshCompiler::CookTriangleMeshData( RawAssets::RawMesh const& rawMesh, TVector<Byte>& outCookedData ) const
        {
            PhysXAllocator allocator;
            PhysXUserErrorCallback errorCallback;

            PxTolerancesScale tolerancesScale;
            tolerancesScale.length = Constants::LengthScale;
            tolerancesScale.speed = Constants::SpeedScale;

            auto pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, allocator, errorCallback );
            auto pCooking = PxCreateCooking( PX_PHYSICS_VERSION, *pFoundation, PxCookingParams( tolerancesScale ) );
            if ( pCooking == nullptr)
            {
                pFoundation->release();
                pFoundation = nullptr;
                Error( "PxCreateCooking failed!" );
                return false;
            }

            // Reflect raw mesh into physx
            //-------------------------------------------------------------------------
            // PhysX meshes require counterclockwise winding

            PxTriangleMeshDesc meshDesc;
            meshDesc.flags = PxMeshFlag::eFLIPNORMALS;
            meshDesc.points.stride = sizeof( F32 ) * 3;
            meshDesc.triangles.stride = sizeof( U32 ) * 3;

            TVector<Float3> vertexData;
            TVector<U32> indexData;

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                // Add the verts
                for ( auto const& vert : geometrySection.m_vertices )
                {
                    vertexData.push_back( vert.m_position );
                }

                // Add the indices - taking into account offset from previously added verts
                for ( auto idx : geometrySection.m_indices )
                {
                    indexData.push_back( meshDesc.points.count + idx );
                }

                meshDesc.points.count += (U32) geometrySection.m_vertices.size();
                meshDesc.triangles.count += geometrySection.GetNumTriangles();
            }

            meshDesc.points.data = vertexData.data();
            meshDesc.triangles.data = indexData.data();

            //-------------------------------------------------------------------------

            outCookedData.clear();
            PhysxMemoryStream stream( outCookedData );
            bool const cookingResult = pCooking->cookTriangleMesh( meshDesc, stream );

            //-------------------------------------------------------------------------

            pCooking->release();
            pFoundation->release();
            pFoundation = nullptr;

            return cookingResult;
        }
    }
}
