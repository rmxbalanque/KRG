#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Camera/CameraSystem.h"
#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    class KRG_ENGINE_CORE_API CameraDebugViewController : public Debug::DebugView
    {
    public:

        void Initialize( CameraSystem* pCameraSystem );
        void Shutdown();

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

    private:

        CameraSystem*       m_pCameraSystem = nullptr;
        F32                 m_recordingTime = 0.1f;
    };
}
#endif