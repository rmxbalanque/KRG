#pragma once

#include "../_Module/API.h"
#include "System/Render/RenderBuffer.h"
#include "System/Render/RenderStates.h"
#include "System/Resource/IResource.h"
#include "System/Core/Math/BoundingVolumes.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------
// Mesh geometry
//-------------------------------------------------------------------------
// The raw vertices/indices for a specific mesh
//
// Notes:
// * KRG uses CCW to determine the facing direction
// * Meshes use the triangle list topology

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API MeshGeometry
    {
        friend class MeshCompiler;
        friend class MeshLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_vertices ), KRG_NVP( m_indices ), KRG_NVP( m_sections ), KRG_NVP( m_vertexBuffer ), KRG_NVP( m_indexBuffer ), KRG_NVP( m_bounds ) );

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

    public:

        struct KRG_ENGINE_RENDER_API GeometrySection
        {
            KRG_SERIALIZE_MEMBERS( m_startIndex, m_numIndices );

            GeometrySection() = default;
            GeometrySection( uint32 startIndex, uint32 numIndices );

            uint32                 m_startIndex = 0;
            uint32                 m_numIndices = 0;
        };

    public:

        inline OBB const& GetBounds() const { return m_bounds; }

        // Vertices
        inline TVector<Byte> const& GetVertexData() const { return m_vertices; }
        inline int32 const GetNumVertices() const { return m_vertexBuffer.m_byteSize / m_vertexBuffer.m_byteStride; }
        inline VertexFormat const& GetVertexFormat() const { return m_vertexBuffer.m_vertexFormat; }
        inline RenderBuffer const& GetVertexBuffer() const { return m_vertexBuffer; }

        // Indices
        inline TVector<uint32> const& GetIndices() const { return m_indices; }
        inline RenderBuffer const& GetIndexBuffer() const { return m_indexBuffer; }

        // Mesh Sections
        inline uint32 GetNumSections() const { return (uint32) m_sections.size(); }
        inline GeometrySection GetSection( uint32 i ) const { KRG_ASSERT( i < GetNumSections() ); return m_sections[i]; }

    protected:

        TVector<Byte>                       m_vertices;
        TVector<uint32>                        m_indices;
        TVector<GeometrySection>            m_sections;
        VertexBuffer                        m_vertexBuffer;
        RenderBuffer                        m_indexBuffer;
        OBB                                 m_bounds;
    };
}