#pragma once

#include "../_Module/API.h"
#include "System/Render/DebugRenderer/DebugRenderStates.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/IRenderer.h"
#include "System/Entity/EntityGlobalSystem.h"

//-------------------------------------------------------------------------

namespace physx
{
    struct PxDebugPoint;
    struct PxDebugLine;
    struct PxDebugTriangle;
    struct PxDebugText;
}

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG::Physics
{
    class PhysicsSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsRenderer final : public Render::IRenderer
    {
        KRG_RENDERER_ID( PhysicsRenderer, Render::RendererPriorityLevel::Debug );

    public:

        bool IsInitialized() const { return m_initialized; }
        bool Initialize( Render::RenderDevice* pRenderDevice, Physics::PhysicsSystem* pPhysicsWorld );
        void Shutdown();
        void Render( Math::Viewport const& viewport ) override final;

    private:

        void DrawPoints( Render::RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugPoint const* pPoints, U32 numPoints );
        void DrawLines( Render::RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugLine const* pLines, U32 numLines );
        void DrawTriangles( Render::RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugTriangle const* pTriangles, U32 numTriangles );
        void DrawStrings( Render::RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugText const* pStrings, U32 numStrings );

    private:

        Render::RenderDevice*                       m_pRenderDevice = nullptr;
        Physics::PhysicsSystem*                      m_pPhysicsWorld = nullptr;

        Render::DebugLineRenderState                m_lineRS;
        Render::DebugPointRenderState               m_pointRS;
        Render::DebugPrimitiveRenderState           m_primitiveRS;
        Render::DebugTextRenderState                m_textRS;

        bool                                        m_initialized = false;
    };
}
#endif