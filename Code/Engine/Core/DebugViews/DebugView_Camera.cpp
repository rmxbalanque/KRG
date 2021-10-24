#include "DebugView_Camera.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Camera
{
    void CameraDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pCameraSystem = pWorld->GetWorldSystem<CameraWorldSystem>();
    }

    void CameraDebugView::Shutdown()
    {
        m_pCameraSystem = nullptr;
    }

    void CameraDebugView::DrawWindows( EntityUpdateContext const& context )
    {
        
    }
}
#endif