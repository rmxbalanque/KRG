#include "TestSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
        UpdatePriorityList const GameTestSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::FrameStart );
}