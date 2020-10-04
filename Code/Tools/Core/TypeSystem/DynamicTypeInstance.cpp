#include "DynamicTypeInstance.h"

//-------------------------------------------------------------------------

namespace KRG
{
    DynamicTypeInstance::DynamicTypeInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo )
        : DynamicPropertyInstance( typeRegistry, pTypeInfo )
    {}
}