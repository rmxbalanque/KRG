#include "MathStringHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    char const* ToString( Axis axis )
    {
        static char const* const axesStrings[] =
        {
            "X",
            "Y",
            "Z",
            "-X",
            "-Y",
            "-Z",
        };

        return axesStrings[(int32) axis];
    }
}