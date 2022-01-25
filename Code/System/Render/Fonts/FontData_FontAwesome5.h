#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/IntegralTypes.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Fonts::FontAwesome5
{
    KRG_SYSTEM_RENDER_API void GetFontCompressedData( TVector<Byte>& compressedData );
}