#pragma once

#include "../_Module/API.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "System/Render/RenderBuffer.h"
#include "System/Render/RenderStates.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Math/BoundingVolumes.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Debug
{
    class DrawingContext;
}

//-------------------------------------------------------------------------
// Base Mesh Data
//-------------------------------------------------------------------------
// The raw vertices/indices for a specific mesh
//
// Notes:
// * KRG uses CCW to determine the facing direction
// * Meshes use the triangle list topology

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API Mesh : public Resource::IResource
    {
        friend class MeshCompiler;
        friend class MeshLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_vertices ), KRG_NVP( m_indices ), KRG_NVP( m_sections ), KRG_NVP( m_materials ), KRG_NVP( m_vertexBuffer ), KRG_NVP( m_indexBuffer ), KRG_NVP( m_bounds ) );

    public:

        struct KRG_ENGINE_RENDER_API GeometrySection
        {
            KRG_SERIALIZE_MEMBERS( m_ID, m_startIndex, m_numIndices );

            GeometrySection() = default;
            GeometrySection( StringID ID, uint32 startIndex, uint32 numIndices );

            StringID                        m_ID;
            uint32                          m_startIndex = 0;
            uint32                          m_numIndices = 0;
        };

    public:

        virtual bool IsValid() const override
        {
            return m_indexBuffer.IsValid() && m_vertexBuffer.IsValid();
        }

        // Bounds
        inline OBB const& GetBounds() const { return m_bounds; }

        // Vertices
        inline TVector<Byte> const& GetVertexData() const { return m_vertices; }
        inline int32 const GetNumVertices() const { return m_vertexBuffer.m_byteSize / m_vertexBuffer.m_byteStride; }
        inline VertexFormat const& GetVertexFormat() const { return m_vertexBuffer.m_vertexFormat; }
        inline RenderBuffer const& GetVertexBuffer() const { return m_vertexBuffer; }

        // Indices
        inline TVector<uint32> const& GetIndices() const { return m_indices; }
        inline int32 const GetNumIndices() const { return (int32) m_indices.size(); }
        inline RenderBuffer const& GetIndexBuffer() const { return m_indexBuffer; }

        // Mesh Sections
        inline TVector<GeometrySection> const& GetSections() const { return m_sections; }
        inline uint32 GetNumSections() const { return (uint32) m_sections.size(); }
        inline GeometrySection GetSection( uint32 i ) const { KRG_ASSERT( i < GetNumSections() ); return m_sections[i]; }

        // Materials
        TVector<TResourcePtr<Material>> const& GetMaterials() const { return m_materials; }

        // Debug
        #if KRG_DEVELOPMENT_TOOLS
        void DrawNormals( Debug::DrawingContext& drawingContext, Transform const& worldTransform ) const;
        #endif

    protected:

        TVector<Byte>                       m_vertices;
        TVector<uint32>                     m_indices;
        TVector<GeometrySection>            m_sections;
        TVector<TResourcePtr<Material>>     m_materials;
        VertexBuffer                        m_vertexBuffer;
        RenderBuffer                        m_indexBuffer;
        OBB                                 m_bounds;
    };
}