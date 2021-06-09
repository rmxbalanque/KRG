#include "MeshCompiler.h"
#include "MeshOptimizer.h"
#include "Tools/Core/Resource/RawAssets/RawMesh.h"
#include "Engine/Render/Mesh/RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct StaticMeshVertex
    {
        Float4  m_position;
        Float4  m_normal;
        Float2  m_UV0;
        Float2  m_UV1;
    };

    struct SkeletalMeshVertex : public StaticMeshVertex
    {
        Int4    m_boneIndices;
        Float4  m_boneWeights;
    };

    //-------------------------------------------------------------------------

    void MeshCompiler::TransferMeshGeometry( RawAssets::RawMesh const& rawMesh, Mesh& mesh ) const
    {
        // Merge all mesh geometries into the main vertex and index buffers
        //-------------------------------------------------------------------------

        uint32 numVertices = 0;
        uint32 numIndices = 0;

        for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
        {
            // Add sub-mesh record
            mesh.m_sections.push_back( Mesh::GeometrySection( StringID( geometrySection.m_name ), numIndices, (uint32) geometrySection.m_indices.size() ) );

            for ( auto idx : geometrySection.m_indices )
            {
                mesh.m_indices.push_back( numVertices + idx );
            }

            numIndices += (uint32) geometrySection.m_indices.size();
            numVertices += (uint32) geometrySection.m_vertices.size();
        }

        // Copy mesh vertex data
        //-------------------------------------------------------------------------

        AABB meshAlignedBounds;
        int32 vertexSize = 0;
        int32 vertexBufferSize = 0;

        if ( rawMesh.IsSkeletalMesh() )
        {
            mesh.m_vertexBuffer.m_vertexFormat = VertexFormat::SkeletalMesh;
            vertexSize = VertexLayoutRegistry::GetDescriptorForFormat( mesh.m_vertexBuffer.m_vertexFormat ).m_byteSize;
            KRG_ASSERT( vertexSize == sizeof( SkeletalMeshVertex ) );

            vertexBufferSize = vertexSize * numVertices;
            mesh.m_vertices.resize( vertexBufferSize );
            auto pVertexMemory = (SkeletalMeshVertex*) mesh.m_vertices.data();

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                for ( auto const& vert : geometrySection.m_vertices )
                {
                    auto pVertex = new( pVertexMemory ) SkeletalMeshVertex();

                    pVertex->m_position = vert.m_position;
                    pVertex->m_normal = vert.m_normal;
                    pVertex->m_UV0 = vert.m_texCoords[0];
                    pVertex->m_UV1 = ( geometrySection.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

                    auto const numInfluences = vert.m_boneIndices.size();
                    KRG_ASSERT( numInfluences <= 4 && vert.m_boneIndices.size() == vert.m_boneWeights.size() );

                    pVertex->m_boneIndices = Int4( InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex );
                    pVertex->m_boneWeights = Float4::Zero;
                    for ( auto i = 0; i < numInfluences; i++ )
                    {
                        pVertex->m_boneIndices[i] = vert.m_boneIndices[i];
                        pVertex->m_boneWeights[i] = vert.m_boneWeights[i];
                    }

                    pVertexMemory++;

                    //-------------------------------------------------------------------------

                    meshAlignedBounds.AddPoint( vert.m_position );
                }
            }
        }
        else
        {
            mesh.m_vertexBuffer.m_vertexFormat = VertexFormat::StaticMesh;
            vertexSize = VertexLayoutRegistry::GetDescriptorForFormat( mesh.m_vertexBuffer.m_vertexFormat ).m_byteSize;
            KRG_ASSERT( vertexSize == sizeof( StaticMeshVertex ) );

            vertexBufferSize = vertexSize * numVertices;
            mesh.m_vertices.resize( vertexBufferSize );
            auto pVertexMemory = (StaticMeshVertex*) mesh.m_vertices.data();

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                for ( auto const& vert : geometrySection.m_vertices )
                {
                    auto pVertex = new( pVertexMemory ) StaticMeshVertex();

                    pVertex->m_position = vert.m_position;
                    pVertex->m_normal = vert.m_normal;
                    pVertex->m_UV0 = vert.m_texCoords[0];
                    pVertex->m_UV1 = ( geometrySection.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

                    pVertexMemory++;

                    //-------------------------------------------------------------------------

                    meshAlignedBounds.AddPoint( vert.m_position );
                }
            }
        }

        // Set Mesh buffer descriptors
        //-------------------------------------------------------------------------

        mesh.m_vertexBuffer.m_byteStride = vertexSize;
        mesh.m_vertexBuffer.m_byteSize = vertexBufferSize;
        mesh.m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
        mesh.m_vertexBuffer.m_usage = RenderBuffer::Usage::GPU_only;

        mesh.m_indexBuffer.m_byteStride = sizeof( uint32 );
        mesh.m_indexBuffer.m_byteSize = (uint32) mesh.m_indices.size() * sizeof( uint32 );
        mesh.m_indexBuffer.m_type = RenderBuffer::Type::Index;
        mesh.m_indexBuffer.m_usage = RenderBuffer::Usage::GPU_only;

        // Calculate bounding volume
        //-------------------------------------------------------------------------
        // TODO: use real algorithm to find minimal bounding box, for now use AABB

        mesh.m_bounds = OBB( meshAlignedBounds );
    }

    void MeshCompiler::OptimizeMeshGeometry( Mesh& mesh ) const
    {
        size_t const vertexSize = (size_t) mesh.m_vertexBuffer.m_byteStride;
        size_t const numVertices = (size_t) mesh.GetNumVertices();

        meshopt_optimizeVertexCache( &mesh.m_indices[0], &mesh.m_indices[0], mesh.m_indices.size(), mesh.m_vertices.size() );

        // Reorder indices for overdraw, balancing overdraw and vertex cache efficiency
        const float kThreshold = 1.01f; // allow up to 1% worse ACMR to get more reordering opportunities for overdraw
        meshopt_optimizeOverdraw( &mesh.m_indices[0], &mesh.m_indices[0], mesh.m_indices.size(), (float*) &mesh.m_vertices[0], numVertices, vertexSize, kThreshold );

        // Vertex fetch optimization should go last as it depends on the final index order
        meshopt_optimizeVertexFetch( &mesh.m_vertices[0], &mesh.m_indices[0], mesh.m_indices.size(), &mesh.m_vertices[0], numVertices, vertexSize );
    }

    void MeshCompiler::SetMeshDefaultMaterials( MeshResourceDescriptor const& descriptor, Mesh& mesh ) const
    {
        mesh.m_materials.reserve( mesh.GetNumSections() );

        for ( auto i = 0u; i < mesh.GetNumSections(); i++ )
        {
            if ( i < descriptor.m_materials.size() )
            {
                mesh.m_materials.push_back( descriptor.m_materials[i] );
            }
            else
            {
                mesh.m_materials.push_back( TResourcePtr<Material>() );
            }
        }
    }

    void MeshCompiler::SetMeshInstallDependencies( Mesh const& mesh, Resource::ResourceHeader& hdr ) const
    {
        for ( auto i = 0u; i < mesh.m_materials.size(); i++ )
        {
            if ( mesh.m_materials[i].IsValid() )
            {
                hdr.AddInstallDependency( mesh.m_materials[i].GetResourceID() );
            }
        }
    }
}