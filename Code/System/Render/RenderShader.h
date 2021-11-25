#pragma once

#include "RenderBuffer.h"
#include "System/Resource/IResource.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API Shader : public Resource::IResource
        {
            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

            KRG_SERIALIZE_MEMBERS( m_cbuffers, m_resourceBindings, m_byteCode );
            KRG_REGISTER_RESOURCE( 'SHDR', "Render Shader" );

        public:

            struct ResourceBinding
            {
                KRG_SERIALIZE_MEMBERS( m_ID, m_slot );

                ResourceBinding() : m_slot( 0 ) {}
                ResourceBinding( uint32 ID, uint32 slot ) : m_ID( ID ), m_slot( slot ) {}

                uint32                 m_ID;
                uint32                 m_slot;
            };

        public:

            virtual ~Shader() { KRG_ASSERT( !m_shaderHandle.IsValid() ); }

            inline PipelineStage GetPipelineStage() const { return m_pipelineStage; }

            inline ShaderHandle const& GetShaderHandle() const { return m_shaderHandle; }
            inline uint32 GetNumConstBuffers() const { return (uint32) m_cbuffers.size(); }
            inline RenderBuffer const& GetConstBuffer( uint32 i ) const { KRG_ASSERT( i < m_cbuffers.size() ); return m_cbuffers[i]; }

            inline bool operator==( Shader const& rhs ) const { return m_shaderHandle.m_pData0 == rhs.m_shaderHandle.m_pData0; }
            inline bool operator!=( Shader const& rhs ) const { return m_shaderHandle.m_pData0 != rhs.m_shaderHandle.m_pData0; }

        protected:

            Shader( PipelineStage stage );
            Shader( PipelineStage stage, uint8 const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

        protected:

            ShaderHandle                        m_shaderHandle;
            TVector<uint8>                      m_byteCode;
            TVector<RenderBuffer>               m_cbuffers;
            TVector<ResourceBinding>            m_resourceBindings;
            PipelineStage                       m_pipelineStage;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API PixelShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'PSDR', "Pixel Shader" );

        public:

            PixelShader() : Shader( PipelineStage::Pixel ) {}
            PixelShader( uint8 const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API GeometryShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'GSDR', "Geometry Shader");

        public:

            GeometryShader() : Shader( PipelineStage::Pixel ) {}
            GeometryShader( uint8 const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API VertexShader : public Shader
        {
            KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Shader ), m_vertexLayoutDesc );
            KRG_REGISTER_RESOURCE( 'VSDR', "Vertex Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

        public:

            VertexShader() : Shader( PipelineStage::Vertex ) {}
            VertexShader( uint8 const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers, VertexLayoutDescriptor const& vertexLayoutDesc );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
            inline VertexLayoutDescriptor const& GetVertexLayoutDesc() const { return m_vertexLayoutDesc; }

        private:

            VertexLayoutDescriptor m_vertexLayoutDesc;
        };

        class KRG_SYSTEM_RENDER_API ComputeShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'CSDR', "Compute Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

        public:

            ComputeShader() : Shader( PipelineStage::Compute ) {}
            ComputeShader( uint8 const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };
    }
}