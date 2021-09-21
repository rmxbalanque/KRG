#pragma once

#include "Engine/Animation/AnimationCommon.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    using NodeIndex = int16;

    //-------------------------------------------------------------------------

    enum class ValueType
    {
        KRG_REGISTER_ENUM

        Unknown = 0,
        Bool,
        ID,
        Int,
        Float,
        Vector,
        Target,
        BoneMask,
        Pose
    };

    //-------------------------------------------------------------------------

    // Used to signify if a node or node output is coming from an active state (i.e. a state we are not transitioning away from)
    enum class BranchState
    {
        Active,
        Inactive,
    };
}