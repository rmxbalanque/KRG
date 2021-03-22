#include "RenderViewportSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    ViewportSystem::ViewportSystem()
    {
        // HACK
        Math::ViewVolume const orthographicVolume( Float2( 640, 480 ), TRange<float>( 0.1f, 100.0f ) );
        m_viewports.push_back( Math::Viewport( Int2( 0, 0 ), Int2( 640, 480 ), orthographicVolume ) );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_devToolsViewportIdx = 0;
        #endif
    }

    void ViewportSystem::ResizePrimaryViewport( Math::Rectangle const& viewportDimensions )
    {
        m_viewports[0].Resize( viewportDimensions.GetSize(), viewportDimensions.GetTopLeft() );
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void ViewportSystem::CreateDevelopmentToolsViewport( Int2 const& viewportDimensions )
    {
        Math::ViewVolume const orthographicVolume( Float2( viewportDimensions ), TRange<float>( 0.1f, 100.0f ) );
        m_viewports.push_back( Math::Viewport( Int2( 0, 0 ), viewportDimensions, orthographicVolume ) );
        m_devToolsViewportIdx = 1;
    }

    void ViewportSystem::ResizeDevelopmentToolsViewport( Int2 const& viewportDimensions )
    {
        m_viewports[1].Resize( viewportDimensions );
    }
    #endif
}