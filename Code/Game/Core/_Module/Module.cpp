#include "Module.h"

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