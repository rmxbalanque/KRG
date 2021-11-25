#pragma once

#include "_Module/API.h"
#include "System/Render/RenderAPI.h"
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
        ~RenderTarget() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

        inline bool IsValid() const { return m_resourceHandle.IsValid(); }

        inline ViewRTHandle const& GetRenderTargetHandle() const { return m_resourceHandle; }
        inline ViewSRVHandle const& GetRenderTargetShaderResourceView() const { return m_renderTargetSRV; }
        inline Int2 const& GetDimensions() const { return m_dimensions; }

        inline bool operator==( RenderTarget const& rhs ) const { return m_resourceHandle.m_pData0 == rhs.m_resourceHandle.m_pData0 && m_resourceHandle.m_pData1 == rhs.m_resourceHandle.m_pData1; }
        inline bool operator!=( RenderTarget const& rhs ) const { return m_resourceHandle.m_pData0 != rhs.m_resourceHandle.m_pData0 && m_resourceHandle.m_pData1 != rhs.m_resourceHandle.m_pData1; }

    protected:

        ViewRTHandle                m_resourceHandle;
        ViewSRVHandle               m_renderTargetSRV;
        Int2                        m_dimensions;
    };
}