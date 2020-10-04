#include "DebugDrawingSystem.h"
#include "System/Core/Debug/DebugDrawingCommands.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        Drawing::ThreadCommandBuffer& DrawingSystem::GetThreadCommandBuffer()
        {
            static thread_local Drawing::ThreadCommandBuffer* pThreadCommandBuffer = nullptr;

            Threading::ScopeLock Lock( m_acquireCommandBufferMutex );

            if ( pThreadCommandBuffer == nullptr )
            {
                pThreadCommandBuffer = KRG::New<Drawing::ThreadCommandBuffer>( Threading::GetCurrentThreadID() );
                m_threadCommandBuffers.emplace_back( pThreadCommandBuffer );
            }

            return *pThreadCommandBuffer;
        }

        DrawingSystem::~DrawingSystem()
        {
            for ( auto pCommandBuffer : m_threadCommandBuffers )
            {
                KRG::Delete( pCommandBuffer );
            }
        }

        void DrawingSystem::ReflectFrameCommandBuffer( Drawing::FrameCommandBuffer& reflectedFrameCommands )
        {
            reflectedFrameCommands.Clear();

            Threading::ScopeLock Lock( m_acquireCommandBufferMutex );
            for ( auto pCommandBuffer : m_threadCommandBuffers )
            {
                reflectedFrameCommands.AddThreadCommands( *pCommandBuffer );
                pCommandBuffer->Clear();
            }
        }
    }
}
#endif