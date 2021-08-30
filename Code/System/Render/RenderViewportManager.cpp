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
        m_viewports.push_back( Viewport( Int2( 0, 0 ), Int2( renderTargetDimensions ), orthographicVolume ) );

        // Create development tools viewport
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_developmentToolsViewport = Viewport( Int2( 0, 0 ), renderTargetDimensions, orthographicVolume );
        #endif
    }

    void ViewportManager::Shutdown()
    {
        // Destroy any created render targets
        for ( auto& viewport : m_viewports )
        {
            if ( viewport.HasCustomRenderTarget() )
            {
                m_pRenderDevice->DestroyRenderTarget( viewport.m_renderTarget );
            }
        }

        m_viewports.clear();

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pRenderDevice != nullptr );
        m_pRenderDevice = nullptr;
    }

    void ViewportManager::UpdateCustomRenderTargets()
    {
        for ( auto& viewport : m_viewports )
        {
            if ( viewport.HasCustomRenderTarget() )
            {
                // Resize render target if needed
                if ( Int2( viewport.m_size ) != viewport.m_renderTarget.GetDimensions() )
                {
                    m_pRenderDevice->ResizeRenderTarget( viewport.m_renderTarget, viewport.m_size );
                }

                // Clear render target and depth stencil textures
                m_pRenderDevice->GetImmediateContext().ClearRenderTargetViews( viewport.m_renderTarget );
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
        for ( auto& viewport : m_viewports )
        {
            Float2 const viewportPosition = viewport.GetTopLeftPosition();
            Float2 const viewportSize = viewport.GetSize();
            Float2 const newViewportPosition = ( viewportPosition / oldWindowDimensions ) * newWindowDimensions;
            Float2 const newViewportSize = ( viewportSize / oldWindowDimensions ) * newWindowDimensions;
            viewport.Resize( newViewportPosition, newViewportSize );
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_developmentToolsViewport.Resize( Int2::Zero, newMainWindowDimensions );
        #endif
    }

    void ViewportManager::ResizePrimaryViewport( Math::Rectangle const& viewportDimensions )
    {
        m_viewports[0].Resize( viewportDimensions );
    }

    void ViewportManager::SetRenderToTexture( int32 viewportIdx, bool enableRenderToTexture )
    {
        KRG_ASSERT( viewportIdx != InvalidIndex && viewportIdx >= 0 && viewportIdx < m_viewports.size() );

        Viewport& viewport = m_viewports[viewportIdx];

        // Do nothing if the state is the same
        if ( viewport.HasCustomRenderTarget() == enableRenderToTexture )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( enableRenderToTexture )
        {
            m_pRenderDevice->CreateRenderTarget( viewport.m_renderTarget, viewport.m_size );
        }
        else // Disable render to texture
        {
            m_pRenderDevice->DestroyRenderTarget( viewport.m_renderTarget );
        }
    }
}