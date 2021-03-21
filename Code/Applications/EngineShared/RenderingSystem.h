#pragma once
#include "System/Render/RendererRegistry.h"
#include "System/Core/Math/Viewport.h"

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
        class SkeletalMeshRenderer;
        class StaticMeshRenderer;

        //-------------------------------------------------------------------------

        class RenderingSystem
        {

        public:

            void Initialize( Render::RendererRegistry* pRegistry );
            void Shutdown();

            void Update( UpdateContext const& ctx, TInlineVector<Math::Viewport, 2> activeViewports );

        private:

            StaticMeshRenderer*                             m_pStaticMeshRenderer = nullptr;
            SkeletalMeshRenderer*                           m_pSkeletalMeshRenderer = nullptr;
            TVector<IRenderer*>                             m_customRenderers;
        };
    }
}