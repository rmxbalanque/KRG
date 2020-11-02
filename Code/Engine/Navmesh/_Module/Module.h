#pragma once

#include "API.h"
#include "Engine/Navmesh/NavmeshSystem.h"
#include "Engine/Navmesh/Debug/NavmeshDebugViewController.h"
#include "Engine/Navmesh/ResourceLoaders/NavmeshLoader.h"
#include "Engine/Core/Modules/IEngineModule.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_ENGINE_NAVMESH_API EngineModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        virtual bool Initialize( ModuleContext& context ) override final;
        virtual void Shutdown( ModuleContext& context ) override final;

    private:

        NavmeshSystem               m_navmeshSystem;
        NavmeshLoader               m_navmeshLoader;

        #if KRG_DEBUG_INSTRUMENTATION
        NavmeshDebugViewController  m_navmeshDebugViewController;
        #endif

        bool                        m_initialized = false;
    };
}