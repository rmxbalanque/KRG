#pragma once

#include "System/Render/Renderers/RendererRegistry.h"
#include "System/Core/Math/Viewport.h"

//-------------------------------------------------------------------------
// KRG Renderer System
//-------------------------------------------------------------------------
// This class allows us to define the exact rendering order and task scheduling for a frame

namespace KRG
{
    class UpdateContext;
    namespace ImGuiX { class ImguiRenderer; }

    //-------------------------------------------------------------------------

    namespace Render
    {
        class ViewportSystem;
        class SkeletalMeshRenderer;
        class StaticMeshRenderer;

        //-------------------------------------------------------------------------

        class RenderingSystem
        {

        public:

            void Initialize( ViewportSystem* pViewportSystem, RendererRegistry* pRegistry );
            void Shutdown();

            void Update( UpdateContext const& ctx );

        private:

            ViewportSystem*                                 m_pViewportSystem = nullptr;
            StaticMeshRenderer*                             m_pStaticMeshRenderer = nullptr;
            SkeletalMeshRenderer*                           m_pSkeletalMeshRenderer = nullptr;
            TVector<IRenderer*>                             m_customRenderers;

            #if KRG_DEVELOPMENT_TOOLS
            ImGuiX::ImguiRenderer*                          m_pImguiRenderer = nullptr;
            #endif
        };
    }
}