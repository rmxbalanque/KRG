#pragma once

#include "Engine/Animation/AnimationCommon.h"
#include "System/Core/Core/IntegralTypes.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    // Used to signify if a node or node output is coming from an active state (i.e. a state we are not transitioning away from)
    enum class BranchState
    {
        Active,
        Inactive,
    };

    //-------------------------------------------------------------------------

    using NodeIndex = int16;
}