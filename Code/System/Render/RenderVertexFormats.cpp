#include "RenderVertexFormats.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        static U32 const g_dataTypeSizes[] =
        {
            1,
            2,
            4,

            1,
            2,
            4,

            4,
            8,
            12,
            16,

            4,
            8,
            12,
            16,

            4,
            8,
            12,
            16,
        };

        static_assert( sizeof( g_dataTypeSizes ) / sizeof( U32 ) == (U32) DataTypeFormat::Unknown, "Mismatched data type and size arrays" );

        U32 GetDataTypeFormatByteSize( DataTypeFormat format )
        {
            U32 const formatIdx = (U32) format;
            KRG_ASSERT( formatIdx < (U32) DataTypeFormat::Unknown );
            U32 const size = g_dataTypeSizes[formatIdx];
            return size;
        }

        //-------------------------------------------------------------------------

        void VertexLayoutDescriptor::CalculateByteSize()
        {
            m_byteSize = 0;
            for ( auto const& vertexElementDesc : m_elementDescriptors )
            {
                m_byteSize += GetDataTypeFormatByteSize( vertexElementDesc.m_format );
            }
        }

        //-------------------------------------------------------------------------

        namespace VertexLayoutRegistry
        {
            VertexLayoutDescriptor GetDescriptorForFormat( VertexFormat format )
            {
                KRG_ASSERT( format != VertexFormat::Unknown );

                VertexLayoutDescriptor layoutDesc;
               
                if ( format == VertexFormat::StaticMesh )
                {
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32B32A32, 0, 0 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Normal, DataTypeFormat::Float_R32G32B32A32, 0, 16 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 0, 32 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 1, 40 ) );

                }
                else if ( format == VertexFormat::SkeletalMesh )
                {
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32B32A32, 0, 0 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Normal, DataTypeFormat::Float_R32G32B32A32, 0, 16 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 0, 32 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 1, 40 ) );

                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::BlendIndex, DataTypeFormat::SInt_R32G32B32A32, 0, 48 ) );
                    layoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::BlendWeight, DataTypeFormat::Float_R32G32B32A32, 0, 64 ) );
                }

                //-------------------------------------------------------------------------

                layoutDesc.CalculateByteSize();
                return layoutDesc;
            }
        }
    }
}