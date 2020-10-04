#include "EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* IEntitySystem::TypeInfoPtr = nullptr;

    //-------------------------------------------------------------------------

    UpdatePriorityList const IEntitySystem::PriorityList = UpdatePriorityList();
}