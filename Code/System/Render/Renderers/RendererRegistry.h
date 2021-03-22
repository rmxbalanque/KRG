#pragma once

#include "System/Render/_Module/API.h"
#include "IRenderer.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_SYSTEM_RENDER_API RendererRegistry
        {
        public:

            RendererRegistry() {}
            ~RendererRegistry();

            void RegisterRenderer( IRenderer* pRenderer );
            void UnregisterRenderer( IRenderer* pRenderer );

            template<typename T>
            inline T* GetRenderer() const
            {
                for ( auto pRenderer : m_registeredRenderers )
                {
                    if ( pRenderer->GetRendererID() == T::RendererID )
                    {
                        return static_cast<T*>( pRenderer );
                    }
                }

                KRG_UNREACHABLE_CODE();
                return nullptr;
            }

            inline TInlineVector<IRenderer*, 20>& GetRegisteredRenderers() { return m_registeredRenderers; }

        private:

            TInlineVector<IRenderer*, 20>     m_registeredRenderers;
        };
    }
}