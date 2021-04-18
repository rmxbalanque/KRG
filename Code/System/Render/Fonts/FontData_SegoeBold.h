#pragma once

#include "../_Module/API.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Types/Containers.h"
//-------------------------------------------------------------------------

namespace KRG::Fonts::SegoeBold
{
    KRG_SYSTEM_RENDER_API void GetFontCompressedData( TVector<Byte>& compressedData );
}