#ifdef _WIN32
#pragma once

#include "System\Core\Types\Function.h"
#include "System\Core\Types\String.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem { class Path; }

//-------------------------------------------------------------------------

namespace KRG
{
    bool EnsureResourceServerIsRunning( FileSystem::Path const& resourceServerExecutablePath );
}

#endif