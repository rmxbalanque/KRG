#include "TypeHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* IRegisteredType::s_pTypeInfo = nullptr;

    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            TTypeHelper<IRegisteredType> TTypeHelper<IRegisteredType>::StaticTypeHelper;
        }

        void RegisterCoreTypeSystemTypes( TypeRegistry& typeRegistry )
        {
            TypeHelpers::TTypeHelper<IRegisteredType>::RegisterType( typeRegistry );
        }

        void UnregisterCoreTypeSystemTypes( TypeRegistry& typeRegistry )
        {
            TypeHelpers::TTypeHelper<IRegisteredType>::UnregisterType( typeRegistry );
        }
    }
}