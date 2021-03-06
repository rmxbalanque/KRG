#include "NavmeshBuilder.h"
#include "Compilers/Physics/PhysicsMeshCompiler.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Resource/RawAssets/RawMesh.h"
#include "Tools/Entity/ToolEntityCollectionConverter.h"
#include "Tools/Entity/ToolEntityCollection.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "Engine/Physics/Components/PhysicsMeshComponent.h"

#include <bfxSystem.h>

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool NavmeshBuilder::Error( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Error, "Navmesh Builder", __FILE__, __LINE__, pFormat, args );
        va_end( args );
        return false;
    }

    void NavmeshBuilder::Warning( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Warning, "Navmesh Builder", __FILE__, __LINE__, pFormat, args );
        va_end( args );
    }

    void NavmeshBuilder::Message( char const* pFormat, ... ) const
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Message, "Navmesh Builder", __FILE__, __LINE__, pFormat, args );
        va_end( args );
    }

    //-------------------------------------------------------------------------

    bool NavmeshBuilder::Build( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, FileSystemPath const& navmeshResourcePath )
    {
        THashMap<DataPath, TVector<Transform>> collisionPrimitives;
        if ( !CollectCollisionPrimitives( ctx, entityCollectionDesc, collisionPrimitives ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( !CollectTriangles( ctx, collisionPrimitives ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        NavmeshData navmeshData;
        if ( !BuildNavmesh( ctx, navmeshData ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( navmeshResourcePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, navmeshResourcePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, NavmeshData::GetStaticResourceTypeID() ) << navmeshData;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool NavmeshBuilder::CollectCollisionPrimitives( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, THashMap<DataPath, TVector<Transform>>& collisionPrimitives )
    {
        EntityModel::ToolEntityCollection toolEntityCollection( ctx.m_typeRegistry );
        if ( !EntityModel::ToolEntityCollectionConverter::ConvertToToolsFormat( ctx.m_typeRegistry, entityCollectionDesc, toolEntityCollection ) )
        {
            return false;
        }

        // Collect all collision geometry
        //-------------------------------------------------------------------------

        auto foundPhysicsComponents = toolEntityCollection.GetAllComponentsOfType( Physics::PhysicsMeshComponent::GetStaticTypeID() );
        for ( auto pPhysicsComponent : foundPhysicsComponents )
        {
            // TODO: see if there is a smart way to avoid using strings for property access
            auto pProperty = pPhysicsComponent->GetProperty( TypeSystem::PropertyPath( "m_pPhysicsMesh" ) );
            if ( pProperty == nullptr )
            {
                return Error( "Cant find 'm_pPhysicsMesh' property on physics mesh component: %s", ctx.m_inputFilePath.c_str() );
            }

            Resource::ResourcePtr geometryPtr = pProperty->GetValue<Resource::ResourcePtr>();
            if ( geometryPtr.IsValid() )
            {
                collisionPrimitives[geometryPtr.GetResourceID().GetDataPath()].emplace_back( pPhysicsComponent->GetWorldTransform() );
            }
        }

        return true;
    }

    bool NavmeshBuilder::CollectTriangles( Resource::CompileContext const& ctx, THashMap<DataPath, TVector<Transform>> const& collisionPrimitives )
    {
        for ( auto const& primitiveDesc : collisionPrimitives )
        {
            // Load descriptor
            //-------------------------------------------------------------------------

            FileSystemPath descFilePath;
            if ( !ctx.ConvertDataPathToFilePath( primitiveDesc.first, descFilePath ) )
            {
                return Error( "Invalid source data path: %s", primitiveDesc.first.c_str() );
            }

            Physics::PhysicsMeshResourceDescriptor resourceDescriptor;
            if ( !ctx.TryReadResourceDescriptorFromFile( descFilePath, resourceDescriptor ) )
            {
                return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            // Load mesh
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
                return Error( "Failed to read mesh from source file: %s" );
            }

            KRG_ASSERT( pRawMesh->IsValid() );

            // Add triangles
            //-------------------------------------------------------------------------

            for ( auto const& transform : primitiveDesc.second )
            {
                auto const scale = transform.GetScale();

                int32 numNegativelyScaledAxes = ( scale.x < 0 ) ? 1 : 0;
                numNegativelyScaledAxes += ( scale.y < 0 ) ? 1 : 0;
                numNegativelyScaledAxes += ( scale.z < 0 ) ? 1 : 0;

                bool const flipWindingDueToScale = Math::IsOdd( numNegativelyScaledAxes );

                //-------------------------------------------------------------------------

                for ( auto const& geometrySection : pRawMesh->GetGeometrySections() )
                {
                    // NavPower expects counterclockwise winding
                    bool flipWinding = geometrySection.m_clockwiseWinding ? true : false;
                    if ( flipWindingDueToScale )
                    {
                        flipWinding = !flipWinding;
                    }

                    //-------------------------------------------------------------------------

                    int32 const numTriangles = geometrySection.GetNumTriangles();
                    int32 const numIndices = (int32) geometrySection.m_indices.size();
                    for ( auto t = 0; t < numTriangles; t++ )
                    {
                        int32 const i = t * 3;
                        KRG_ASSERT( i <= numIndices - 3 );

                        // NavPower expects counterclockwise winding
                        int32 const index0 = geometrySection.m_indices[flipWinding ? i + 2 : i];
                        int32 const index1 = geometrySection.m_indices[i + 1];
                        int32 const index2 = geometrySection.m_indices[flipWinding ? i : i + 2];

                        // Add triangle
                        auto& buildFace = m_buildFaces.emplace_back( bfx::BuildFace() );
                        buildFace.m_type = bfx::WALKABLE_FACE;
                        buildFace.m_verts[0] = ToBfx( transform.TransformPoint( geometrySection.m_vertices[index0].m_position ) );
                        buildFace.m_verts[1] = ToBfx( transform.TransformPoint( geometrySection.m_vertices[index1].m_position ) );
                        buildFace.m_verts[2] = ToBfx( transform.TransformPoint( geometrySection.m_vertices[index2].m_position ) );
                    }
                }
            }
        }

        return true;
    }

    bool NavmeshBuilder::BuildNavmesh( Resource::CompileContext const& ctx, NavmeshData& navmeshData )
    {
        if ( m_buildFaces.empty() )
        {
            return true;
        }

        //-------------------------------------------------------------------------

        bfx::CustomAllocator* pAllocator = bfx::CreateDLMallocAllocator();
        bfx::SystemCreate( bfx::SystemParams( 1.0f, bfx::Z_UP ), pAllocator );
        bfx::RegisterBuilderSystem();
        bfx::SystemStart();

        //-------------------------------------------------------------------------

        bfx::PlatformParams const platformParams;

        //-------------------------------------------------------------------------

        auto& buildParams = m_buildParams.emplace_back( bfx::BuildParams() );
        buildParams.m_voxSize = 0.1f;
        buildParams.m_height = 2.0f;
        buildParams.m_radius = 0.4f;
        buildParams.m_step = 0.2f;
        buildParams.m_additionalInwardsSmoothingDist = 0.0f;
        buildParams.m_optimizeForAxisAligned = false;
        buildParams.m_dropOffRadius = 0.2f;
        buildParams.m_maxWalkableSlope = 45.0f;
        buildParams.m_leaveSmallIslandsTouchingPortals = true;
        buildParams.m_minIslandSurfaceArea = 16.f;
        buildParams.m_useEnhancedTerrainTracking = true;
        buildParams.m_tessellateForPathingAccuracy = false;

        //-------------------------------------------------------------------------

        bfx::SurfaceNavigationInput surfaceInput;
        surfaceInput.m_globalParams.m_maxNumCores = 16;

        surfaceInput.m_pFaces = m_buildFaces.data();
        surfaceInput.m_numFaces = (uint32) m_buildFaces.size();
        surfaceInput.m_pParams = m_buildParams.data();
        surfaceInput.m_numParams = (uint32) m_buildParams.size();

        //-------------------------------------------------------------------------

        //bfx::EnableBuildLog( "D:\\buildlog.bfx_log", true );
        bfx::NavGraphImage* pGraphImage = bfx::CreateNavGraphImage( surfaceInput, platformParams );

        // Copy graph image data into the resource
        navmeshData.m_graphImage.resize( pGraphImage->GetNumBytes() );
        memcpy( navmeshData.m_graphImage.data(), pGraphImage->GetPtr(), pGraphImage->GetNumBytes() );

        bfx::DestroyNavGraphImage( pGraphImage );

        //-------------------------------------------------------------------------

        bfx::SystemStop();
        bfx::SystemDestroy();
        bfx::DestroyAllocator( pAllocator );

        return true;
    }
}