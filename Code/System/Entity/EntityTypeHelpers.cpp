#include "EntityTypeHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            TTypeHelper<Entity> TTypeHelper<Entity>::StaticTypeHelper;
            TTypeHelper<EntityComponent> TTypeHelper<EntityComponent>::StaticTypeHelper;
            TTypeHelper<SpatialEntityComponent> TTypeHelper<SpatialEntityComponent>::StaticTypeHelper;
            TTypeHelper<IEntitySystem> TTypeHelper<IEntitySystem>::StaticTypeHelper;
        }

        void RegisterCoreEntityTypes( TypeRegistry& typeRegistry )
        {
            TypeHelpers::TTypeHelper<Entity>::RegisterType( typeRegistry );
            TypeHelpers::TTypeHelper<EntityComponent>::RegisterType( typeRegistry );
            TypeHelpers::TTypeHelper<SpatialEntityComponent>::RegisterType( typeRegistry );
            TypeHelpers::TTypeHelper<IEntitySystem>::RegisterType( typeRegistry );
        }

        void UnregisterCoreEntityTypes( TypeRegistry& typeRegistry )
        {
            TypeHelpers::TTypeHelper<IEntitySystem>::UnregisterType( typeRegistry );
            TypeHelpers::TTypeHelper<SpatialEntityComponent>::UnregisterType( typeRegistry );
            TypeHelpers::TTypeHelper<EntityComponent>::UnregisterType( typeRegistry );
            TypeHelpers::TTypeHelper<Entity>::UnregisterType( typeRegistry );
        }
    }
}