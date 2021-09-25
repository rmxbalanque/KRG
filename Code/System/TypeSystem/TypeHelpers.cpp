#include "TypeHelpers.h"
#include "TypeInfo.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* IRegisteredType::s_pTypeInfo = nullptr;

    namespace TypeSystem::TypeHelpers
    {
        TTypeHelper<IRegisteredType> TTypeHelper<IRegisteredType>::StaticTypeHelper;
    }
}