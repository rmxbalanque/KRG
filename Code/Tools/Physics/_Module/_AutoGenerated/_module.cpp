//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Tools\Physics\_Module\Module.h"
#include "System/Resource/ResourceSystem.h"
#include "D:\Kruger\Code\Tools\Physics\_Module\_AutoGenerated\2693261070.cpp"
#include "D:\Kruger\Code\Tools\Physics\_Module\_AutoGenerated\4278505152.cpp"

//-------------------------------------------------------------------------

void KRG::Physics::ToolsModule::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor>::RegisterType( typeRegistry );
}

void KRG::Physics::ToolsModule::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor>::UnregisterType( typeRegistry );
}

