#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Core.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fonts
    {
        KRG_SYSTEM_RENDER_API void GetDecompressedFontData( Byte const* pSourceData, TVector<Byte>& fontData );
    }
}