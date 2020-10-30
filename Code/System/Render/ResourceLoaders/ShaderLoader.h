#pragma once

#include "../_Module/API.h"
#include "System/Render/RenderShader.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_SYSTEM_RENDER_API ShaderLoader : public Resource::ResourceLoader
        {
        public:

            ShaderLoader() : m_pRenderDevice( nullptr )
            {
                m_loadableTypes.push_back( PixelShader::GetStaticResourceTypeID() );
                m_loadableTypes.push_back( VertexShader::GetStaticResourceTypeID() );
            }

            inline void SetRenderDevice( RenderDevice* pRenderDevice )
            {
                KRG_ASSERT( pRenderDevice != nullptr );
                m_pRenderDevice = pRenderDevice;
            }

            inline void ClearRenderDevice() { m_pRenderDevice = nullptr; }

        private:

            virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
            virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const final;

        private:

            RenderDevice* m_pRenderDevice;
        };
    }
}