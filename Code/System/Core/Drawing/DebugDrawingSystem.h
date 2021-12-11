#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Drawing/DebugDrawing.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Drawing
{
    class KRG_SYSTEM_CORE_API DrawingSystem
    {

    public:

        DrawingSystem() = default;
        ~DrawingSystem();

        // Empty all per thread buffers
        void Reset();

        // Returns a per-thread drawing context, this removes the need for constantly calling get thread command buffer
        inline DrawContext GetDrawingContext() { return DrawContext( GetThreadCommandBuffer() ); }

        // Reflects all the individual per-thread buffers into a single supplied frame command buffer. Clears all thread buffers.
        void ReflectFrameCommandBuffer( Seconds const deltaTime, FrameCommandBuffer& reflectedFrameCommands );

    private:

        ThreadCommandBuffer& GetThreadCommandBuffer();

    private:

        TVector<ThreadCommandBuffer*>       m_threadCommandBuffers;
        Threading::Mutex                    m_commandBufferMutex;
    };
}
#endif