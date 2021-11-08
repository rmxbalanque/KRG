#include "DebugDrawingSystem.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Debug
{
    Drawing::ThreadCommandBuffer& DrawingSystem::GetThreadCommandBuffer()
    {
        Threading::ScopeLock Lock( m_commandBufferMutex );

        auto const threadID = Threading::GetCurrentThreadID();

        // Check for an already created buffer for this thread
        for ( auto& pThreadBuffer : m_threadCommandBuffers )
        {
            if ( pThreadBuffer->GetThreadID() == threadID )
            {
                return *pThreadBuffer;
            }
        }

        // Create a new buffer
        auto& pThreadBuffer = m_threadCommandBuffers.emplace_back( KRG::New<Drawing::ThreadCommandBuffer>( threadID ) );
        return *pThreadBuffer;
    }

    DrawingSystem::~DrawingSystem()
    {
        for ( auto& pBuffer : m_threadCommandBuffers )
        {
            KRG::Delete( pBuffer );
        }
    }

    void DrawingSystem::ReflectFrameCommandBuffer( Drawing::FrameCommandBuffer& reflectedFrameCommands )
    {
        reflectedFrameCommands.Clear();

        Threading::ScopeLock Lock( m_commandBufferMutex );
        for ( auto& pThreadBuffer : m_threadCommandBuffers )
        {
            reflectedFrameCommands.AddThreadCommands( *pThreadBuffer );
            pThreadBuffer->Clear();
        }
    }

    void DrawingSystem::Reset()
    {
        Threading::ScopeLock Lock( m_commandBufferMutex );
        for ( auto& pThreadBuffer : m_threadCommandBuffers )
        {
            pThreadBuffer->Clear();
        }
    }
}
#endif