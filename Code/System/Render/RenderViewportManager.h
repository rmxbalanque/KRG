#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------
// Viewport management
//-------------------------------------------------------------------------
// Primary viewport and window management in Kruger

namespace KRG::Render
{
    class RenderDevice;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RENDER_API ViewportManager : public ISystem
    {
    public:

        KRG_SYSTEM_ID( ViewportManager );

    public:

        void Initialize( RenderDevice* pRenderDevice );
        void Shutdown();

        //-------------------------------------------------------------------------
        // Global View Management
        //-------------------------------------------------------------------------

        void UpdateMainWindowSize( Int2 newMainWindowDimensions );

        //-------------------------------------------------------------------------
        // Engine Viewports
        //-------------------------------------------------------------------------

        void ResizePrimaryViewport( Math::Rectangle const& viewportDimensions );

        inline TInlineVector<Math::Viewport, 2>& GetActiveViewports() { return m_viewports; }
        inline TInlineVector<Math::Viewport, 2> const& GetActiveViewports() const { return m_viewports; }

        //-------------------------------------------------------------------------
        // Development UI
        //-------------------------------------------------------------------------
        // There is always an orthographic "development tools" viewport created that is the same size as the main window

        #if KRG_DEVELOPMENT_TOOLS
        inline Math::Viewport const& GetDevelopmentToolsViewport() const { return m_developmentToolsViewport; }
        #endif

    private:

        RenderDevice*                               m_pRenderDevice = nullptr;
        TInlineVector<Math::Viewport, 2>            m_viewports;

        #if KRG_DEVELOPMENT_TOOLS
        Math::Viewport                              m_developmentToolsViewport;
        #endif
    };
}