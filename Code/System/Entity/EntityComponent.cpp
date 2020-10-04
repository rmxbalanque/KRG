#include "EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* EntityComponent::TypeInfoPtr = nullptr;

    //-------------------------------------------------------------------------

    EntityComponent::~EntityComponent()
    {
        KRG_ASSERT( m_status == Status::Unloaded );
    }
}