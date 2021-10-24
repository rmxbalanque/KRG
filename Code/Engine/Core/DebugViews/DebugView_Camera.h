#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Camera/CameraWorldSystem.h"
#include "Engine/Core/Entity/Debug/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Camera
{
    class KRG_ENGINE_CORE_API CameraDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( CameraDebugView );

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context ) override;

    private:

        CameraWorldSystem*      m_pCameraSystem = nullptr;
        float                   m_recordingTime = 0.1f;
    };
}
#endif