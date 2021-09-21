#pragma once

#include "System\TypeSystem\TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    enum class CoordinateSpace
    {
        KRG_REGISTER_ENUM

        Bone = 0,
        Character,
        World
    };
}