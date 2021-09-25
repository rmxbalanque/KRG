#include "DebugView_Camera.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Camera
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