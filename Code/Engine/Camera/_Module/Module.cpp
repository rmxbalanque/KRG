#include "Module.h"
#include "System/Core/Systems/SystemRegistry.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Camera
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        context.RegisterWorldSystem( &m_cameraSystem );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_cameraDebugViewController.Initialize( &m_cameraSystem );
        context.RegisterDebugView( &m_cameraDebugViewController );
        #endif

        return true;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        #if KRG_DEVELOPMENT_TOOLS
        context.UnregisterDebugView( &m_cameraDebugViewController );
        m_cameraDebugViewController.Shutdown();
        #endif

        //-------------------------------------------------------------------------

        context.UnregisterWorldSystem( &m_cameraSystem );
    }
}