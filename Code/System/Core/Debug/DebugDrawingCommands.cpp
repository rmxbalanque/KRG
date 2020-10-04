#include "DebugDrawingCommands.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        namespace Drawing
        {
            void FrameCommandBuffer::AddThreadCommands( ThreadCommandBuffer const& threadCommands )
            {
                // TODO:
                // Broad-phase culling
                // Sort transparent and depth test off primitives by distance to camera
                // Sort text by font

                m_opaqueDepthOn.Append( threadCommands.GetOpaqueDepthTestEnabledBuffer() );
                m_opaqueDepthOff.Append( threadCommands.GetOpaqueDepthTestDisabledBuffer() );
                m_transparentDepthOn.Append( threadCommands.GetTransparentDepthTestEnabledBuffer() );
                m_transparentDepthOff.Append( threadCommands.GetTransparentDepthTestDisabledBuffer() );
            }
        }
    }
}
#endif