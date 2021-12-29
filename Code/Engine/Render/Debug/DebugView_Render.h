#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    class RendererWorldSystem;
    class LightComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API RenderDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( RenderDebugView );

    public:

        static void DrawRenderVisualizationModesMenu( EntityWorld const* pWorld );

    public:

        RenderDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawOverlayElements( EntityUpdateContext const& context ) override;

        void DrawRenderMenu( EntityUpdateContext const& context );

    private:

        RendererWorldSystem*            m_pWorldRendererSystem = nullptr;
    };
}
#endif