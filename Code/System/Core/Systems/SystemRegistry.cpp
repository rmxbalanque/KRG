#include "SystemRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    SystemRegistry::~SystemRegistry()
    {
        KRG_ASSERT( m_registeredSystems.empty() );
    }

    void SystemRegistry::RegisterSystem( ISystem* pSystem )
    {
        KRG_ASSERT( pSystem != nullptr && !VectorContains( m_registeredSystems, pSystem ) );
        m_registeredSystems.emplace_back( pSystem );
    }

    void SystemRegistry::UnregisterSystem( ISystem* pSystem )
    {
        auto iter = VectorFind( m_registeredSystems, pSystem );
        KRG_ASSERT( iter != m_registeredSystems.end() );
        m_registeredSystems.erase_unsorted( iter );
    }
}