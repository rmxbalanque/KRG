#include "RenderTarget.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    RenderTarget::~RenderTarget()
    {
        KRG_ASSERT( !m_resourceHandle.IsValid() );
        KRG_ASSERT( m_pTexture == nullptr );
    }
}