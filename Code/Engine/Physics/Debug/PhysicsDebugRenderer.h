#pragma once

#include "../_Module/API.h"
#include "Engine/Render/Renderers/DebugRenderStates.h"
#include "Engine/Render/IRenderer.h"
#include "System/Render/RenderDevice.h"

//-------------------------------------------------------------------------

namespace physx
{
    struct PxDebugPoint;
    struct PxDebugLine;
    struct PxDebugTriangle;
    struct PxDebugText;
}

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Physics
{
    class PhysicsSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsRenderer final : public Render::IRenderer
    {
        KRG_RENDERER_ID( PhysicsRenderer, Render::RendererPriorityLevel::Debug );

    public:

        bool IsInitialized() const { return m_initialized; }
        bool Initialize( Render::RenderDevice* pRenderDevice );
        void Shutdown();
        void RenderWorld( Render::Viewport const& viewport, EntityWorld* pWorld ) override final;

    private:

        void DrawPoints( Render::RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugPoint const* pPoints, uint32 numPoints );
        void DrawLines( Render::RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugLine const* pLines, uint32 numLines );
        void DrawTriangles( Render::RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugTriangle const* pTriangles, uint32 numTriangles );
        void DrawStrings( Render::RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugText const* pStrings, uint32 numStrings );

    private:

        Render::RenderDevice*                       m_pRenderDevice = nullptr;

        Render::DebugLineRenderState                m_lineRS;
        Render::DebugPointRenderState               m_pointRS;
        Render::DebugPrimitiveRenderState           m_primitiveRS;
        Render::DebugTextRenderState                m_textRS;

        bool                                        m_initialized = false;
    };
}
#endif