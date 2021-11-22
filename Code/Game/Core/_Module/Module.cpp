#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Game
    {
        bool GameModule::Initialize( ModuleContext& context )
        {
            return true;
        }

        void GameModule::Shutdown( ModuleContext& context )
        {
        }
    }
}