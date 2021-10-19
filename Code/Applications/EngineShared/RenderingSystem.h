#pragma once

#include "System/Render/RendererRegistry.h"

//-------------------------------------------------------------------------
// KRG Renderer System
//-------------------------------------------------------------------------
// This class allows us to define the exact rendering order and task scheduling for a frame

namespace KRG
{
    class UpdateContext;

    //-------------------------------------------------------------------------

    namespace Render
    {
        class RenderDevice;
        class ViewportManager;
        class MeshRenderer;
        class ImguiRenderer;

        //-------------------------------------------------------------------------

        class RenderingSystem
        {

        public:

            void Initialize( RenderDevice* pRenderDevice, ViewportManager* pViewportManager, RendererRegistry* pRegistry );
            void Shutdown();

            void Update( UpdateContext const& ctx );

        private:

            RenderDevice*                                   m_pRenderDevice = nullptr;

            ViewportManager*                                m_pViewportManager = nullptr;
            MeshRenderer*                                   m_pMeshRenderer = nullptr;
            TVector<IRenderer*>                             m_customRenderers;

            #if KRG_DEVELOPMENT_TOOLS
            ImguiRenderer*                                  m_pImguiRenderer = nullptr;
            #endif
        };
    }
}