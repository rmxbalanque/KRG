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

        virtual bool Initialize( ModuleContext& ctx ) = 0;
        virtual void Shutdown( ModuleContext& ctx ) = 0;
    };
}