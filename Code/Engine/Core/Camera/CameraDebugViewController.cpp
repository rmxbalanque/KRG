#include "CameraDebugViewController.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    void CameraDebugViewController::Initialize( CameraSystem* pCameraSystem )
    {
        KRG_ASSERT( pCameraSystem != nullptr );
        m_pCameraSystem = pCameraSystem;
    }

    void CameraDebugViewController::Shutdown()
    {
        m_pCameraSystem = nullptr;
    }

    void CameraDebugViewController::DrawWindows( UpdateContext const& context )
    {
        
    }
}
#endif