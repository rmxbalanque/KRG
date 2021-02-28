#include "EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* EntityComponent::StaticTypeInfo = nullptr;

    //-------------------------------------------------------------------------

    EntityComponent::~EntityComponent()
    {
        KRG_ASSERT( m_status == Status::Unloaded );
        KRG_ASSERT( m_isRegisteredWithEntity == false && m_isRegisteredWithWorld == false );
    }
}