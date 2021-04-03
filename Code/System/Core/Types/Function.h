#pragma once
#include <EASTL/functional.h>

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T> using TFunction = eastl::function<T>;
}