#include "RenderViewportManager.h"
#include "RenderDevice/RenderDevice.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void ViewportManager::Initialize( RenderDevice* pRenderDevice )
    {
        KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
        m_pRenderDevice = pRenderDevice;

        // Create primary viewport
        //-------------------------------------------------------------------------

        Int2 const renderTargetDimensions = m_pRenderDevice->GetMainRenderTargetDimensions();
        Math::ViewVolume const orthographicVolume( Float2( renderTargetDimensions ), FloatRange( 0.1f, 100.0f ) );
        m_viewports.push_back( KRG::New<Viewport>( Int2( 0, 0 ), Int2( renderTargetDimensions ), orthographicVolume ) );

        // Create development tools viewport
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_developmentToolsViewport = Viewport( Int2( 0, 0 ), renderTargetDimensions, orthographicVolume );
        #endif
    }

    void ViewportManager::Shutdown()
    {
        // Destroy any created render targets
        for ( auto& pViewport : m_viewports )
        {
            if ( pViewport->m_renderTarget.IsValid() )
            {
                m_pRenderDevice->DestroyRenderTarget( pViewport->m_renderTarget );
            }

            KRG::Delete( pViewport );
        }

        m_viewports.clear();

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pRenderDevice != nullptr );
        m_pRenderDevice = nullptr;
    }

    void ViewportManager::UpdateCustomRenderTargets()
    {
        for ( auto& pViewport : m_viewports )
        {
            if ( pViewport->m_renderTarget.IsValid() )
            {
                // Resize render target if needed
                if ( Int2( pViewport->m_size ) != pViewport->m_renderTarget.GetDimensions() )
                {
                    m_pRenderDevice->ResizeRenderTarget( pViewport->m_renderTarget, pViewport->m_size );
                }

                // Clear render target and depth stencil textures
                m_pRenderDevice->GetImmediateContext().ClearRenderTargetViews( pViewport->m_renderTarget );
            }
        }
    }

    void ViewportManager::UpdateMainWindowSize( Int2 newMainWindowDimensions )
    {
        KRG_ASSERT( newMainWindowDimensions.m_x > 0 && newMainWindowDimensions.m_y > 0 );

        Float2 const newWindowDimensions = Float2( newMainWindowDimensions );
        Float2 const oldWindowDimensions = Float2( m_pRenderDevice->GetMainRenderTargetDimensions() );
        m_pRenderDevice->ResizeMainRenderTarget( newMainWindowDimensions );

        //-------------------------------------------------------------------------

        // TODO: add info whether viewports are in absolute size or proportional, right now it's all proportional
        for ( auto& pViewport : m_viewports )
        {
            Float2 const viewportPosition = pViewport->GetTopLeftPosition();
            Float2 const viewportSize = pViewport->GetSize();
            Float2 const newViewportPosition = ( viewportPosition / oldWindowDimensions ) * newWindowDimensions;
            Float2 const newViewportSize = ( viewportSize / oldWindowDimensions ) * newWindowDimensions;
            pViewport->Resize( newViewportPosition, newViewportSize );
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_developmentToolsViewport.Resize( Int2::Zero, newMainWindowDimensions );
        #endif
    }

    void ViewportManager::ResizePrimaryViewport( Math::Rectangle const& viewportDimensions )
    {
        m_viewports[0]->Resize( viewportDimensions );
    }

    void ViewportManager::SetUseCustomRenderTargetForViewport( int32 viewportIdx, bool enableRenderToTexture )
    {
        KRG_ASSERT( viewportIdx != InvalidIndex && viewportIdx >= 0 && viewportIdx < m_viewports.size() );

        Viewport* pViewport = m_viewports[viewportIdx];

        // Do nothing if the state is the same
        if ( pViewport->m_renderTarget.IsValid() == enableRenderToTexture )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( enableRenderToTexture )
        {
            m_pRenderDevice->CreateRenderTarget( pViewport->m_renderTarget, pViewport->m_size );
        }
        else // Disable render to texture
        {
            m_pRenderDevice->DestroyRenderTarget( pViewport->m_renderTarget );
        }
    }

    ShaderResourceView const& ViewportManager::GetViewportRenderTargetTextureSRV( int32 viewportIdx ) const
    {
        KRG_ASSERT( viewportIdx != InvalidIndex && viewportIdx >= 0 && viewportIdx < m_viewports.size() );
        return m_viewports[viewportIdx]->m_renderTarget.GetRenderTargetShaderResourceView();
    }

    RenderTarget const& ViewportManager::GetRenderTargetForViewport( int32 viewportIdx ) const
    {
        KRG_ASSERT( viewportIdx != InvalidIndex && viewportIdx >= 0 && viewportIdx < m_viewports.size() );

        auto pViewport = m_viewports[viewportIdx];
        if ( pViewport->m_renderTarget.IsValid() )
        {
            return pViewport->m_renderTarget;
        }
        else
        {
            return m_pRenderDevice->GetMainRenderTarget();
        }
    }
}