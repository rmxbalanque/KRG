#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Game
    {
        bool GameModule::Initialize( ModuleContext& context )
        {
            EntityWorld* pWorld = context.GetEntityWorld();


            return true;
        }

        void GameModule::Shutdown( ModuleContext& context )
        {
        }
    }
}