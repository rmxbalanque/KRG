#pragma once

#include "../_Module/API.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        class KRG_SYSTEM_CORE_API DrawingSystem : public ISystem
        {
        public:

            KRG_SYSTEM_ID( DrawingSystem );

        public:

            DrawingSystem() = default;
            ~DrawingSystem();

            // Returns a per-thread drawing context, this removes the need for constantly calling get thread command buffer
            inline DrawingContext GetDrawingContext() { return DrawingContext( GetThreadCommandBuffer() ); }

            // Reflects all the individual per-thread buffers into a single frame command buffer
            void ReflectFrameCommandBuffer( Drawing::FrameCommandBuffer& reflectedFrameCommands );

        private:

            Drawing::ThreadCommandBuffer& GetThreadCommandBuffer();

        private:

            TVector<Drawing::ThreadCommandBuffer*>      m_threadCommandBuffers;
            Threading::Mutex                            m_acquireCommandBufferMutex;
        };
    }
}
#endif