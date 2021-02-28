#pragma once

#include "API.h"
#include "Engine/Navmesh/NavmeshSystem.h"
#include "Engine/Navmesh/NavmeshWorldSystem.h"
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
        NavmeshWorldSystem          m_navmeshWorldSystem;
        NavmeshLoader               m_navmeshLoader;

        #if KRG_DEVELOPMENT_TOOLS
        NavmeshDebugViewController  m_navmeshDebugViewController;
        #endif

        bool                        m_initialized = false;
    };
}