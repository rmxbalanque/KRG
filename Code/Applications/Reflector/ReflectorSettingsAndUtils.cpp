#include "ReflectorSettingsAndUtils.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            static char const* g_macroNames[] =
            {
                "KRG_REFLECTION_IGNORE_HEADER",
                "KRG_REGISTER_MODULE",
                "KRG_REGISTER_ENUM",
                "KRG_REGISTER_TYPE",
                "KRG_REGISTER_RESOURCE",
                "KRG_REGISTER_VIRTUAL_RESOURCE",
                "KRG_REGISTER_ENTITY_COMPONENT",
                "KRG_REGISTER_ENTITY_SYSTEM",
                "EXPOSE",
            };

            //-------------------------------------------------------------------------

            char const* GetReflectionMacroText( ReflectionMacro macro )
            {
                U32 const macroIdx = (U32) macro;
                KRG_ASSERT( macroIdx < (U32) ReflectionMacro::NumMacros );
                return g_macroNames[macroIdx];
            }
        }
    }
}