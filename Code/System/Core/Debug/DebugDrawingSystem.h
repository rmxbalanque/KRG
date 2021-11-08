#pragma once

#include "../_Module/API.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Debug
{
    class KRG_SYSTEM_CORE_API DrawingSystem
    {

    public:

        DrawingSystem() = default;
        ~DrawingSystem();

        // Empty all per thread buffers
        void Reset();

        // Returns a per-thread drawing context, this removes the need for constantly calling get thread command buffer
        inline DrawingContext GetDrawingContext() { return DrawingContext( GetThreadCommandBuffer() ); }

        // Reflects all the individual per-thread buffers into a single supplied frame command buffer. Clears all thread buffers.
        void ReflectFrameCommandBuffer( Drawing::FrameCommandBuffer& reflectedFrameCommands );

    private:

        Drawing::ThreadCommandBuffer& GetThreadCommandBuffer();

    private:

        TVector<Drawing::ThreadCommandBuffer*>      m_threadCommandBuffers;
        Threading::Mutex                            m_commandBufferMutex;
    };
}
#endif