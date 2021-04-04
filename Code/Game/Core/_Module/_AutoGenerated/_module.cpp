//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Game\Core\_Module\Module.h"
#include "System/Resource/ResourceSystem.h"
#include "D:\Kruger\Code\Game\Core\_Module\_AutoGenerated\1326117132.cpp"
#include "D:\Kruger\Code\Game\Core\_Module\_AutoGenerated\1415443842.cpp"
#include "D:\Kruger\Code\Game\Core\_Module\_AutoGenerated\1784434565.cpp"
#include "D:\Kruger\Code\Game\Core\_Module\_AutoGenerated\2131064025.cpp"
#include "D:\Kruger\Code\Game\Core\_Module\_AutoGenerated\2593147084.cpp"

//-------------------------------------------------------------------------

void KRG::Game::GameModule::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::DefaultPlayerController>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct>::RegisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestFlags::RegisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct>::RegisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestComponent_InternalEnum::RegisterEnum( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestComponent_InternalTest_InternalEnum::RegisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::GameTestSystem>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestSystem>::RegisterType( typeRegistry );
}

void KRG::Game::GameModule::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestSystem>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::GameTestSystem>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent>::UnregisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestComponent_InternalTest_InternalEnum::UnregisterEnum( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestComponent_InternalEnum::UnregisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct>::UnregisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_TestFlags::UnregisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::DefaultPlayerController>::UnregisterType( typeRegistry );
}

