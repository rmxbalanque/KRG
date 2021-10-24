#pragma once

#include "Engine/Render/RendererRegistry.h"

//-------------------------------------------------------------------------
// KRG Renderer System
//-------------------------------------------------------------------------
// This class allows us to define the exact rendering order and task scheduling for a frame

namespace KRG
{
    class UpdateContext;
    class EntityWorldManager;

    //-------------------------------------------------------------------------

    namespace Render
    {
        class RenderDevice;
        class ViewportManager;
        class WorldRenderer;
        class DebugRenderer;
        class ImguiRenderer;

        //-------------------------------------------------------------------------

        class RenderingSystem
        {

        public:

            void Initialize( RenderDevice* pRenderDevice, ViewportManager* pViewportManager, RendererRegistry* pRegistry, EntityWorldManager* pWorldManager );
            void Shutdown();

            void Update( UpdateContext const& ctx );

        private:

            RenderDevice*                                   m_pRenderDevice = nullptr;

            ViewportManager*                                m_pViewportManager = nullptr;
            EntityWorldManager*                             m_pWorldManager = nullptr;

            WorldRenderer*                                  m_pWorldRenderer = nullptr;
            TVector<IRenderer*>                             m_customRenderers;

            #if KRG_DEVELOPMENT_TOOLS
            DebugRenderer*                                  m_pDebugRenderer = nullptr;
            ImguiRenderer*                                  m_pImguiRenderer = nullptr;
            #endif
        };
    }
}