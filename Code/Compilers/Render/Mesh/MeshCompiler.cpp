#ifdef _WIN32
#include "MeshCompiler.h"
#include "MeshOptimizer.h"
#include "Tools/Resource/RawAssets/RawMesh.h"
#include "System/Render/Mesh/MeshGeometry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void MeshCompiler::TransferMeshGeometry( RawAssets::RawMesh const& rawMesh, MeshGeometry& mesh ) const
        {
            // Merge all mesh geometries into the main vertex and index buffers
            //-------------------------------------------------------------------------

            U32 numVertices = 0;
            U32 numIndices = 0;

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                // Add sub-mesh record
                mesh.m_sections.push_back( MeshGeometry::GeometrySection( numIndices, (U32) geometrySection.m_indices.size() ) );

                for ( auto idx : geometrySection.m_indices )
                {
                    mesh.m_indices.push_back( numVertices + idx );
                }

                numIndices += (U32) geometrySection.m_indices.size();
                numVertices += (U32) geometrySection.m_vertices.size();
            }

            // Copy mesh vertex data
            //-------------------------------------------------------------------------

            AABB meshAlignedBounds;
            S32 vertexSize = 0;
            S32 vertexBufferSize = 0;

            if ( rawMesh.IsSkeletalMesh() )
            {
                mesh.m_vertexBuffer.m_vertexFormat = VertexFormat::SkeletalMesh;
                vertexSize = VertexLayoutRegistry::GetDescriptorForFormat( mesh.m_vertexBuffer.m_vertexFormat ).m_byteSize;
                KRG_ASSERT( vertexSize == sizeof( MeshGeometry::SkeletalMeshVertex ) );

                vertexBufferSize = vertexSize * numVertices;
                mesh.m_vertices.resize( vertexBufferSize );
                auto pVertexMemory = (MeshGeometry::SkeletalMeshVertex*) mesh.m_vertices.data();

                for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
                {
                    for ( auto const& vert : geometrySection.m_vertices )
                    {
                        auto pVertex = new( pVertexMemory ) MeshGeometry::SkeletalMeshVertex();

                        pVertex->m_position = vert.m_position;
                        pVertex->m_normal = vert.m_normal;
                        pVertex->m_UV0 = vert.m_texCoords[0];
                        pVertex->m_UV1 = ( rawMesh.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

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
                KRG_ASSERT( vertexSize == sizeof( MeshGeometry::StaticMeshVertex ) );

                vertexBufferSize = vertexSize * numVertices;
                mesh.m_vertices.resize( vertexBufferSize );
                auto pVertexMemory = ( MeshGeometry::StaticMeshVertex* ) mesh.m_vertices.data();

                for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
                {
                    for ( auto const& vert : geometrySection.m_vertices )
                    {
                        auto pVertex = new( pVertexMemory ) MeshGeometry::StaticMeshVertex();

                        pVertex->m_position = vert.m_position;
                        pVertex->m_normal = vert.m_normal;
                        pVertex->m_UV0 = vert.m_texCoords[0];
                        pVertex->m_UV1 = ( rawMesh.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

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

            mesh.m_indexBuffer.m_byteStride = sizeof( U32 );
            mesh.m_indexBuffer.m_byteSize = (U32) mesh.m_indices.size() * sizeof( U32 );
            mesh.m_indexBuffer.m_type = RenderBuffer::Type::Index;
            mesh.m_indexBuffer.m_usage = RenderBuffer::Usage::GPU_only;

            // Calculate bounding volume
            //-------------------------------------------------------------------------
            // TODO: use real algorithm to find minimal bounding box, for now use AABB

            mesh.m_bounds = OBB( meshAlignedBounds );
        }

        void MeshCompiler::OptimizeMeshGeometry( MeshGeometry& mesh ) const
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
    }
}
#endif