#include "NavmeshBuilder.h"
#include "Tools/Physics/ResourceDescriptors/ResourceDescriptor_PhysicsMesh.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Core/Resource/RawAssets/RawMesh.h"
#include "Tools/Entity/EntityToolAccessor.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "Engine/Physics/Components/Component_PhysicsMesh.h"
#include "Engine/Core/Entity/EntityCollection.h"
#include "Engine/Core/Entity/EntityCollectionDescriptor.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include <bfxSystem.h>

//-------------------------------------------------------------------------

namespace KRG
{
    template<>
    struct TEntityToolAccessor<Physics::PhysicsMeshComponent>
    {
        TEntityToolAccessor( Physics::PhysicsMeshComponent* pType )
            : m_pType( pType )
        {}

        inline ResourceID const& GetMeshResourceID() { return m_pType->m_pPhysicsMesh.GetResourceID(); }

    protected:

        Physics::PhysicsMeshComponent* m_pType = nullptr;
    };
}

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

    bool NavmeshBuilder::Build( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, FileSystem::Path const& navmeshResourcePath, NavmeshComponent const* pNavmeshComponent )
    {
        THashMap<ResourcePath, TVector<Transform>> collisionPrimitives;
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

        KRG_ASSERT( pNavmeshComponent != nullptr );

        NavmeshData navmeshData;
        if ( !BuildNavmesh( ctx, pNavmeshComponent, navmeshData ) )
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
            archive << Resource::ResourceHeader( s_version, NavmeshData::GetStaticResourceTypeID() ) << navmeshData;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool NavmeshBuilder::CollectCollisionPrimitives( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, THashMap<ResourcePath, TVector<Transform>>& collisionPrimitives )
    {
        EntityModel::EntityCollection const collectionInstance( ctx.m_typeRegistry, UUID::GenerateID(), entityCollectionDesc );

        // Update all spatial transforms
        //-------------------------------------------------------------------------
        // We need to do this since we dont update world transforms when loading a collection

        for ( auto pEntity : collectionInstance.GetEntities() )
        {
            if ( pEntity->IsSpatialEntity() )
            {
                pEntity->SetWorldTransform( pEntity->GetWorldTransform() );
            }
        }

        // Collect all collision geometry
        //-------------------------------------------------------------------------

        auto foundPhysicsComponents = entityCollectionDesc.GetComponentsOfType( ctx.m_typeRegistry, Physics::PhysicsMeshComponent::GetStaticTypeID() );
        for ( auto const& searchResult : foundPhysicsComponents )
        {
            int32 const entityIdx = entityCollectionDesc.FindEntityIndex( searchResult.m_pEntity->m_name );
            KRG_ASSERT( entityIdx != InvalidIndex );

            int32 const componentIdx = entityCollectionDesc.GetEntityDescriptors()[entityIdx].FindComponentIndex( searchResult.m_pComponent->m_name );
            KRG_ASSERT( componentIdx != InvalidIndex );

            Entity const* pEntity = collectionInstance.GetEntities()[entityIdx];
            KRG_ASSERT( pEntity != nullptr );

            auto pPhysicsComponent = Cast<Physics::PhysicsMeshComponent>( pEntity->GetComponents()[componentIdx] );
            KRG_ASSERT( pPhysicsComponent != nullptr );

            TEntityToolAccessor<Physics::PhysicsMeshComponent> accessor( pPhysicsComponent );
            ResourceID geometryResourceID = accessor.GetMeshResourceID();
            if ( geometryResourceID.IsValid() )
            {
                 collisionPrimitives[geometryResourceID.GetResourcePath()].emplace_back( pPhysicsComponent->GetWorldTransform() );
            }
        }

        return true;
    }

    bool NavmeshBuilder::CollectTriangles( Resource::CompileContext const& ctx, THashMap<ResourcePath, TVector<Transform>> const& collisionPrimitives )
    {
        for ( auto const& primitiveDesc : collisionPrimitives )
        {
            // Load descriptor
            //-------------------------------------------------------------------------

            FileSystem::Path descFilePath;
            if ( !ctx.ConvertResourcePathToFilePath( primitiveDesc.first, descFilePath ) )
            {
                return Error( "Invalid source data path (%s) for physics mesh descriptor", primitiveDesc.first.c_str() );
            }

            Physics::PhysicsMeshResourceDescriptor resourceDescriptor;
            if ( !TryReadResourceDescriptorFromFile( ctx.m_typeRegistry, descFilePath, resourceDescriptor ) )
            {
                return Error( "Failed to read physics mesh resource descriptor from file: %s", descFilePath.c_str() );
            }

            // Load mesh
            //-------------------------------------------------------------------------

            FileSystem::Path meshFilePath;
            if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_meshPath, meshFilePath ) )
            {
                return Error( "Invalid source data path (%) in physics mesh descriptor: %s", resourceDescriptor.m_meshPath.c_str(), descFilePath.c_str() );
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

                int32 numNegativelyScaledAxes = ( scale.m_x < 0 ) ? 1 : 0;
                numNegativelyScaledAxes += ( scale.m_y < 0 ) ? 1 : 0;
                numNegativelyScaledAxes += ( scale.m_z < 0 ) ? 1 : 0;

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

    bool NavmeshBuilder::BuildNavmesh( Resource::CompileContext const& ctx, NavmeshComponent const* pNavmeshComponent, NavmeshData& navmeshData )
    {
        KRG_ASSERT( pNavmeshComponent != nullptr );

        if ( m_buildFaces.empty() )
        {
            return true;
        }

        //-------------------------------------------------------------------------

        bfx::CustomAllocator* pAllocator = bfx::CreateDLMallocAllocator();
        bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), pAllocator );
        bfx::RegisterBuilderSystem();
        bfx::SystemStart();

        //-------------------------------------------------------------------------

        TVector<bfx::BuildParams> layerBuildParams;

        // Default Layer
        auto& defaultLayerBuildParams = layerBuildParams.emplace_back( bfx::BuildParams() );
        defaultLayerBuildParams.m_voxSize = pNavmeshComponent->m_defaultLayerBuildSettings.m_voxSize;
        defaultLayerBuildParams.m_height = pNavmeshComponent->m_defaultLayerBuildSettings.m_height;
        defaultLayerBuildParams.m_radius = pNavmeshComponent->m_defaultLayerBuildSettings.m_radius;
        defaultLayerBuildParams.m_step = pNavmeshComponent->m_defaultLayerBuildSettings.m_step;
        defaultLayerBuildParams.m_additionalInwardsSmoothingDist = pNavmeshComponent->m_defaultLayerBuildSettings.m_additionalInwardsSmoothingDist;
        defaultLayerBuildParams.m_optimizeForAxisAligned = pNavmeshComponent->m_defaultLayerBuildSettings.m_optimizeForAxisAligned;
        defaultLayerBuildParams.m_dropOffRadius = pNavmeshComponent->m_defaultLayerBuildSettings.m_dropOffRadius;
        defaultLayerBuildParams.m_maxWalkableSlope = pNavmeshComponent->m_defaultLayerBuildSettings.m_maxWalkableSlope;
        defaultLayerBuildParams.m_leaveSmallIslandsTouchingPortals = pNavmeshComponent->m_defaultLayerBuildSettings.m_leaveSmallIslandsTouchingPortals;
        defaultLayerBuildParams.m_minIslandSurfaceArea = pNavmeshComponent->m_defaultLayerBuildSettings.m_minIslandSurfaceArea;
        defaultLayerBuildParams.m_useEnhancedTerrainTracking = pNavmeshComponent->m_defaultLayerBuildSettings.m_useEnhancedTerrainTracking;
        defaultLayerBuildParams.m_tessellateForPathingAccuracy = pNavmeshComponent->m_defaultLayerBuildSettings.m_tessellateForPathingAccuracy;

        // Additional Layers
        for ( auto const& layerSettings : pNavmeshComponent->m_additionalLayerBuildSettings )
        {
            auto& buildParams = layerBuildParams.emplace_back( bfx::BuildParams() );
            buildParams.m_voxSize = layerSettings.m_voxSize;
            buildParams.m_height = layerSettings.m_height;
            buildParams.m_radius = layerSettings.m_radius;
            buildParams.m_step = layerSettings.m_step;
            buildParams.m_additionalInwardsSmoothingDist = layerSettings.m_additionalInwardsSmoothingDist;
            buildParams.m_optimizeForAxisAligned = layerSettings.m_optimizeForAxisAligned;
            buildParams.m_dropOffRadius = layerSettings.m_dropOffRadius;
            buildParams.m_maxWalkableSlope = layerSettings.m_maxWalkableSlope;
            buildParams.m_leaveSmallIslandsTouchingPortals = layerSettings.m_leaveSmallIslandsTouchingPortals;
            buildParams.m_minIslandSurfaceArea = layerSettings.m_minIslandSurfaceArea;
            buildParams.m_useEnhancedTerrainTracking = layerSettings.m_useEnhancedTerrainTracking;
            buildParams.m_tessellateForPathingAccuracy = layerSettings.m_tessellateForPathingAccuracy;
        }

        //-------------------------------------------------------------------------

        bfx::SurfaceNavigationInput surfaceInput;
        surfaceInput.m_globalParams.m_maxNumCores = 16;

        surfaceInput.m_pFaces = m_buildFaces.data();
        surfaceInput.m_numFaces = (uint32) m_buildFaces.size();
        surfaceInput.m_pParams = layerBuildParams.data();
        surfaceInput.m_numParams = (uint32) layerBuildParams.size();

        surfaceInput.m_globalParams.m_enableMulticoreBuild = true;
        surfaceInput.m_globalParams.m_maxNumCores = Threading::GetProcessorInfo().m_numPhysicalCores;

        //-------------------------------------------------------------------------

        if ( pNavmeshComponent->m_enableBuildLogging )
        {
            bfx::EnableBuildLog( "D:\\buildlog.bfx_log", true );
        }
        bfx::NavGraphImage* pGraphImage = bfx::CreateNavGraphImage( surfaceInput, bfx::PlatformParams() );

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