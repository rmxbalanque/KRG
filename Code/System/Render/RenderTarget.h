#pragma once

#include "_Module/API.h"
#include "RenderTexture.h"
#include "System/Render/RenderResourceHandle.h"
#include "System/Resource/IResource.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_SYSTEM_RENDER_API RenderTarget
    {
        friend class RenderDevice;

    public:

        RenderTarget() = default;
        ~RenderTarget();

        inline bool IsValid() const { return m_resourceHandle.IsValid(); }

        inline ResourceHandle const& GetRenderTargetHandle() const { return m_resourceHandle; }
        inline Texture const* GetRenderTargetTexture() const { KRG_ASSERT( m_pTexture != nullptr && m_pTexture->IsValid() ); return m_pTexture; }
        inline Int2 const& GetDimensions() const { return m_dimensions; }

        inline bool operator==( RenderTarget const& rhs ) const { return m_resourceHandle.m_pHandle == rhs.m_resourceHandle.m_pHandle; }
        inline bool operator!=( RenderTarget const& rhs ) const { return m_resourceHandle.m_pHandle != rhs.m_resourceHandle.m_pHandle; }

    protected:

        ResourceHandle  m_resourceHandle;
        Texture*        m_pTexture = nullptr;
        Int2            m_dimensions;
    };
}