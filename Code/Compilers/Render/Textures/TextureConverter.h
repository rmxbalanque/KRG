#ifdef _WIN32
#pragma once

#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool ConvertTexture( char const* pFilename, TVector<Byte>& rawData );
    }
}

#endif