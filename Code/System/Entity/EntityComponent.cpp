#include "EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* EntityComponent::s_pTypeInfo = nullptr;

    //-------------------------------------------------------------------------

    EntityComponent::~EntityComponent()
    {
        KRG_ASSERT( m_status == Status::Unloaded );
        KRG_ASSERT( m_isRegisteredWithEntity == false && m_isRegisteredWithWorld == false );
    }
}