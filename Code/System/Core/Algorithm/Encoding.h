#pragma once
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Encoding
{
    //-------------------------------------------------------------------------
    // Base 64 Encoding
    //-------------------------------------------------------------------------

    namespace Base64
    {
        KRG_SYSTEM_CORE_API TVector<Byte> Encode( Byte const* pDataToEncode, size_t dataSize );
        KRG_SYSTEM_CORE_API TVector<Byte> Decode( Byte const* pDataToDecode, size_t dataSize );
    }

    //-------------------------------------------------------------------------
    // Base 85 Encoding
    //-------------------------------------------------------------------------

    namespace Base85
    {
        KRG_SYSTEM_CORE_API TVector<Byte> Encode( Byte const* pDataToEncode, size_t dataSize );
        KRG_SYSTEM_CORE_API TVector<Byte> Decode( Byte const* pDataToDecode, size_t dataSize );
    }
}