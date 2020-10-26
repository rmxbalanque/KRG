#include "EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* IEntitySystem::StaticTypeInfo = nullptr;

    //-------------------------------------------------------------------------

    UpdatePriorityList const IEntitySystem::PriorityList = UpdatePriorityList();
}