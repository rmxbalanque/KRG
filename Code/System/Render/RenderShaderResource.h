#pragma once

#include "_Module/API.h"
#include "System/Render/RenderResourceHandle.h"

//-------------------------------------------------------------------------
// Shader Resource View
//-------------------------------------------------------------------------
// Abstraction for a shader resource view

namespace KRG::Render
{
    class KRG_SYSTEM_RENDER_API ShaderResourceView final
    {
        friend class RenderDevice;

    public:

        ~ShaderResourceView() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

        inline bool IsValid() const { return m_resourceHandle.IsValid(); }

        ResourceHandle const& GetResourceHandle() const { return m_resourceHandle; }

        inline bool operator==( ShaderResourceView const& rhs ) const { return m_resourceHandle.m_pData0 == rhs.m_resourceHandle.m_pData0; }
        inline bool operator!=( ShaderResourceView const& rhs ) const { return m_resourceHandle.m_pData0 != rhs.m_resourceHandle.m_pData0; }

    protected:

        ResourceHandle  m_resourceHandle;
    };
}