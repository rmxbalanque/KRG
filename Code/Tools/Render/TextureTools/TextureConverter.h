#pragma once

#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool ConvertTexture( char const* pFilename, TVector<Byte>& rawData );
}