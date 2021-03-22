#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_SYSTEM_RENDER_API ViewportSystem : public ISystem
    {
    public:

        KRG_SYSTEM_ID( ViewportSystem );

    public:

        ViewportSystem();

        // HACK
        void ResizePrimaryViewport( Math::Rectangle const& viewportDimensions );

        //-------------------------------------------------------------------------
        // Engine viewports
        //-------------------------------------------------------------------------

        inline bool HasActiveViewports() const { return !m_viewports.empty(); }

        inline TInlineVector<Math::Viewport, 2>& GetActiveViewports()
        {
            return m_viewports;
        }

        inline TInlineVector<Math::Viewport, 2> const& GetActiveViewports() const
        {
            return m_viewports;
        }

        //-------------------------------------------------------------------------
        // Development UI
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void CreateDevelopmentToolsViewport( Int2 const& viewportDimensions );
        void ResizeDevelopmentToolsViewport( Int2 const& viewportDimensions );

        inline Math::Viewport const& GetDevelopmentToolsViewport() const 
        {
            KRG_ASSERT( m_devToolsViewportIdx != InvalidIndex );
            return m_viewports[m_devToolsViewportIdx]; 
        }
        #endif

    private:

        TInlineVector<Math::Viewport, 2>            m_viewports;

        #if KRG_DEVELOPMENT_TOOLS
        int32                                       m_devToolsViewportIdx = InvalidIndex;
        #endif
    };
}