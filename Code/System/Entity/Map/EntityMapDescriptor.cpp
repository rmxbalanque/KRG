#include "EntityMapDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    bool EntityMapDescriptor::IsValid() const
    {
        return m_collectionTemplate.IsValid();
    }
}