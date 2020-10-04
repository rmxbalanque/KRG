#include "Easing.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        namespace Easing
        {
            static const EasingFuncPtr g_easingFunctions[] =
            {
                EaseInLinear,
                EaseInQuad,
                EaseOutQuad,
                EaseInOutQuad,
                EaseOutInQuad,
                EaseInCubic,
                EaseOutCubic,
                EaseInOutCubic,
                EaseOutInCubic,
                EaseInQuart,
                EaseOutQuart,
                EaseInOutQuart,
                EaseOutInQuart,
                EaseInQuint,
                EaseOutQuint,
                EaseInOutQuint,
                EaseOutInQuint,
                EaseInSine,
                EaseOutSine,
                EaseInOutSine,
                EaseOutInSine,
                EaseInExpo,
                EaseOutExpo,
                EaseInOutExpo,
                EaseOutInExpo,
                EaseInCirc,
                EaseOutCirc,
                EaseInOutCirc,
                EaseOutInCirc,
            };

            //-------------------------------------------------------------------------

            KRG::F32 EvaluateEasingFunction( Type type, F32 parameter )
            {
                return g_easingFunctions[(U32) type]( parameter );
            }
        }
    }
}