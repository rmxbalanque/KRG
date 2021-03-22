#include "RendererRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        RendererRegistry::~RendererRegistry()
        {
            KRG_ASSERT( m_registeredRenderers.empty() );
        }

        void RendererRegistry::RegisterRenderer( IRenderer* pRenderer )
        {
            KRG_ASSERT( pRenderer != nullptr && !VectorContains( m_registeredRenderers, pRenderer ) );
            m_registeredRenderers.emplace_back( pRenderer );
        }

        void RendererRegistry::UnregisterRenderer( IRenderer* pRenderer )
        {
            auto iter = VectorFind( m_registeredRenderers, pRenderer );
            KRG_ASSERT( iter != m_registeredRenderers.end() );
            m_registeredRenderers.erase_unsorted( iter );
        }
    }
}