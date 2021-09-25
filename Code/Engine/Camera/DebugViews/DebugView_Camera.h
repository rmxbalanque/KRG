#pragma once

#include "Engine/Camera/_Module/API.h"
#include "Engine/Camera/CameraWorldSystem.h"
#include "Engine/Core/DebugTools/DebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Camera
{
    class KRG_ENGINE_CAMERA_API CameraDebugViewController : public Debug::DebugView
    {
    public:

        void Initialize( CameraWorldSystem* pCameraSystem );
        void Shutdown();

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

    private:

        CameraWorldSystem*      m_pCameraSystem = nullptr;
        float                   m_recordingTime = 0.1f;
    };
}
#endif