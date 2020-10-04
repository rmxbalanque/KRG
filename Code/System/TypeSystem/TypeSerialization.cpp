#include "TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        char const* const TypeSerializationSettings::TypeKey = "TypeID"; // Does not use 'm_' prefix since it is not a member/property
    }
}