#pragma once
#include "_Module/API.h"
#include "RenderViewport.h"
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

        void UpdateCustomRenderTargets();

        //-------------------------------------------------------------------------
        // Global View Management
        //-------------------------------------------------------------------------

        void UpdateMainWindowSize( Int2 newMainWindowDimensions );

        //-------------------------------------------------------------------------
        // Engine Viewports
        //-------------------------------------------------------------------------

        inline Viewport const* GetPrimaryViewport() const { return m_viewports[0]; }
        void ResizePrimaryViewport( Math::Rectangle const& viewportDimensions );

        inline int32 GetNumViewports() const { return (int32) m_viewports.size(); }
        inline TInlineVector<Viewport*, 2>& GetActiveViewports() { return m_viewports; }
        inline TInlineVector<Viewport*, 2> const& GetActiveViewports() const { return m_viewports; }

        //-------------------------------------------------------------------------
        // Render Targets
        //-------------------------------------------------------------------------

        RenderTarget const& GetRenderTargetForViewport( int32 viewportIdx ) const;
        void SetUseCustomRenderTargetForViewport( int32 viewportIdx, bool enableRenderToTexture );
        ShaderResourceView const& GetViewportRenderTargetTextureSRV( int32 viewportIdx ) const;

        //-------------------------------------------------------------------------
        // Development UI
        //-------------------------------------------------------------------------
        // There is always an orthographic "development tools" viewport created that is the same size as the main window

        #if KRG_DEVELOPMENT_TOOLS
        inline Viewport const& GetDevelopmentToolsViewport() const { return m_developmentToolsViewport; }
        #endif

    private:

        RenderDevice*                           m_pRenderDevice = nullptr;
        TInlineVector<Viewport*, 2>             m_viewports;

        #if KRG_DEVELOPMENT_TOOLS
        Viewport                                m_developmentToolsViewport;
        #endif
    };
}