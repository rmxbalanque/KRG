#include "EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityComponent::~EntityComponent()
    {
        KRG_ASSERT( m_status == Status::Unloaded );
        KRG_ASSERT( m_isRegisteredWithEntity == false && m_isRegisteredWithWorld == false );
    }
}