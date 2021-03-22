#pragma once

#include "API.h"
#include "Engine/Camera/CameraWorldSystem.h"
#include "Engine/Camera/DebugViews/CameraDebugViewController.h"
#include "Engine/Core/Modules/IEngineModule.h"

//-------------------------------------------------------------------------

namespace KRG::Camera
{
    class KRG_ENGINE_CAMERA_API EngineModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        virtual bool Initialize( ModuleContext& context ) override final;
        virtual void Shutdown( ModuleContext& context ) override final;

    private:
    
        CameraWorldSystem                               m_cameraSystem;

        #if KRG_DEVELOPMENT_TOOLS
        CameraDebugViewController                       m_cameraDebugViewController;
        #endif
    };
}