#pragma once

#include "../_Module/API.h"
#include "ISystem.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API SystemRegistry
    {
    public:

        SystemRegistry() {}
        ~SystemRegistry();

        void RegisterSystem( ISystem* pSystem );
        void UnregisterSystem( ISystem* pSystem );

        template<typename T>
        inline T* GetSystem() const
        {
            static_assert( std::is_base_of<KRG::ISystem, T>::value, "T is not derived from ISystem" );

            for ( auto pSystem : m_registeredSystems )
            {
                if ( pSystem->GetSystemID() == T::s_systemID )
                {
                    return static_cast<T*>( pSystem );
                }
            }

            KRG_UNREACHABLE_CODE();
            return nullptr;
        }

    private:

        TInlineVector<ISystem*, 20>     m_registeredSystems;
    };
}