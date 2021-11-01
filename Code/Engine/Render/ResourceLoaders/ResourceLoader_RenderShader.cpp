#include "ResourceLoader_RenderShader.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool ShaderLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr && archive.IsValid() );

        // Get shader resource
        Shader* pShaderResource = nullptr;
        auto const shaderResourceTypeID = resID.GetResourceTypeID();

        if ( shaderResourceTypeID == VertexShader::GetStaticResourceTypeID() )
        {
            auto pVertexShaderResource = KRG::New<VertexShader>();
            archive >> *pVertexShaderResource;
            pShaderResource = pVertexShaderResource;
        }
        else if ( shaderResourceTypeID == PixelShader::GetStaticResourceTypeID() )
        {
            auto pPixelShaderResource = KRG::New<PixelShader>();
            archive >> *pPixelShaderResource;
            pShaderResource = pPixelShaderResource;
        }
        else
        {
            return false;
        }

        KRG_ASSERT( pShaderResource != nullptr );

        // Create shader
        m_pRenderDevice->LockDevice();
        m_pRenderDevice->CreateShader( *pShaderResource );
        m_pRenderDevice->UnlockDevice();
        pResourceRecord->SetResourceData( pShaderResource );
        return true;
    }

    void ShaderLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr );

        auto pShaderResource = pResourceRecord->GetResourceData<Shader>();
        if ( pShaderResource != nullptr )
        {
            m_pRenderDevice->LockDevice();
            m_pRenderDevice->DestroyShader( *pShaderResource );
            m_pRenderDevice->UnlockDevice();
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}