#include "CameraDebugViewController.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    void CameraDebugViewController::Initialize( CameraWorldSystem* pCameraSystem )
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