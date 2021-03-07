#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ModuleContext;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API IModule
    {
    public:

        virtual ~IModule() = default;

        // Required initialization and shutdown stages, called at the start of engine initialization and just before exiting
        virtual bool Initialize( ModuleContext& ctx ) = 0;
        virtual void Shutdown( ModuleContext& ctx ) = 0;

        // Optional stages if this module required any global resources to be loaded
        virtual void LoadModuleResources( Resource::ResourceSystem& resourceSystem ) {}
        virtual bool OnEngineResourceLoadingComplete() { return true; }
        virtual void UnloadModuleResources( Resource::ResourceSystem& resourceSystem ) {}
    };
}