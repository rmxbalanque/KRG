#pragma once

#include "_Module/API.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        enum class DataSemantic : uint8
        {
            Position = 0,
            Normal,
            Tangent,
            BiTangent,
            Color,
            TexCoord,
            BlendIndex,
            BlendWeight,

            None,
        };

        enum class DataTypeFormat : uint8
        {
            UInt_R8 = 0,
            UInt_R8G8,
            UInt_R8G8B8A8,

            UNorm_R8,
            UNorm_R8G8,
            UNorm_R8G8B8A8,

            UInt_R32,
            UInt_R32G32,
            UInt_R32G32B32,
            UInt_R32G32B32A32,

            SInt_R32,
            SInt_R32G32,
            SInt_R32G32B32,
            SInt_R32G32B32A32,

            Float_R32,
            Float_R32G32,
            Float_R32G32B32,
            Float_R32G32B32A32,

            Unknown,
        };

        KRG_SYSTEM_RENDER_API uint32 GetDataTypeFormatByteSize( DataTypeFormat format );

        //-------------------------------------------------------------------------

        enum class VertexFormat
        {
            Unknown = 0,
            StaticMesh,
            SkeletalMesh,
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
                uint16                 m_semanticIndex = 0;
                uint16                 m_offset = 0;
            };

        public:

            inline bool IsValid() const { return m_byteSize > 0 && !m_elementDescriptors.empty(); }
            void CalculateByteSize();

        public:

            TInlineVector<ElementDescriptor, 6>     m_elementDescriptors;
            uint32                                     m_byteSize = 0;             // The total byte size per vertex
        };

        //-------------------------------------------------------------------------

        namespace VertexLayoutRegistry
        {
            KRG_SYSTEM_RENDER_API VertexLayoutDescriptor GetDescriptorForFormat( VertexFormat format );
        }
    }
}