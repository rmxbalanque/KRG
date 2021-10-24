#pragma once

#include "API.h"
#include "Engine/Navmesh/NavmeshSystem.h"
#include "Engine/Navmesh/NavmeshWorldSystem.h"
#include "Engine/Navmesh/Debug/NavmeshDebugRenderer.h"
#include "Engine/Navmesh/ResourceLoaders/ResourceLoader_Navmesh.h"
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

        #if KRG_DEVELOPMENT_TOOLS
        NavmeshDebugRenderer        m_navmeshRenderer;
        #endif

        bool                        m_initialized = false;
    };
}