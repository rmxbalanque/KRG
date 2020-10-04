#ifdef _WIN32
#include "../UUID.h"
#include <Objbase.h>

//-------------------------------------------------------------------------

namespace KRG
{
    UUID UUID::GenerateID()
    {
        UUID newID;
        static_assert( sizeof( GUID ) == sizeof( UUID ), "Size mismatch for KRG GUID vs Win32 GUID" );
        CoCreateGuid( (GUID*) &newID );
        return newID;
    }
}
#endif