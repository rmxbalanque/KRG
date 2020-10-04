#pragma once

#include "System/Core/Memory/Memory.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/stringbuffer.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/writer.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/prettywriter.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/document.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        struct RapidJsonAllocator
        {
            inline void* Malloc( size_t size ) { return KRG::Alloc( size ); }
            inline void* Realloc( void* pOriginalPtr, size_t originalSize, size_t newSize ) { return KRG::Realloc( pOriginalPtr, newSize ); }
            inline static void Free( void* pData ) { KRG::Free( pData ); }
        };

        typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<char>, RapidJsonAllocator> RapidJsonStringBuffer;
    }
}