#pragma once

#include "_Module/API.h"
#include "RenderAPI.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_SYSTEM_RENDER_API uint32 GetDataTypeFormatByteSize( DataTypeFormat format );

    //-------------------------------------------------------------------------

    enum class VertexFormat
    {
        Unknown = 0,
        None,
        StaticMesh,
        SkeletalMesh,
    };

    // CPU format for the static mesh vertex - this is what the mesh compiler fills the vertex data array with
    struct StaticMeshVertex
    {
        Float4  m_position;
        Float4  m_normal;
        Float2  m_UV0;
        Float2  m_UV1;
    };

    // CPU format for the skeletal mesh vertex - this is what the mesh compiler fills the vertex data array with
    struct SkeletalMeshVertex : public StaticMeshVertex
    {
        Int4    m_boneIndices;
        Float4  m_boneWeights;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_RENDER_API VertexLayoutDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_elementDescriptors, m_byteSize );

        struct ElementDescriptor
        {
            KRG_SERIALIZE_MEMBERS( m_semantic, m_format, m_semanticIndex, m_offset );

            ElementDescriptor() = default;

            ElementDescriptor( DataSemantic semantic, DataTypeFormat format, uint16 semanticIndex, uint16 offset ) : m_semantic( semantic )
                , m_format( format )
                , m_semanticIndex( semanticIndex )
                , m_offset( offset )
            {}

            DataSemantic        m_semantic = DataSemantic::None;
            DataTypeFormat      m_format = DataTypeFormat::Unknown;
            uint16              m_semanticIndex = 0;
            uint16              m_offset = 0;
        };

    public:

        inline bool IsValid() const { return m_byteSize > 0 && !m_elementDescriptors.empty(); }
        void CalculateByteSize();

    public:

        TInlineVector<ElementDescriptor, 6>     m_elementDescriptors;
        uint32                                  m_byteSize = 0;             // The total byte size per vertex
    };

    //-------------------------------------------------------------------------

    namespace VertexLayoutRegistry
    {
        KRG_SYSTEM_RENDER_API VertexLayoutDescriptor GetDescriptorForFormat( VertexFormat format );
    }
}