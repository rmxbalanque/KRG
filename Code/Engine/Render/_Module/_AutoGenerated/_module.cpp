//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Engine\Render\_Module\Module.h"
#include "System/Resource/ResourceSystem.h"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\1101229334.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\1173081372.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\2895451261.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\35218945.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\421821798.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\523519686.cpp"
#include "D:\Kruger\Code\Engine\Render\_Module\_AutoGenerated\662370697.cpp"

//-------------------------------------------------------------------------

void KRG::Render::EngineModule::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MeshComponent>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshComponent>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::WorldRendererSystem>::RegisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_Render_Mobility::RegisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshComponent>::RegisterType( typeRegistry );
}

void KRG::Render::EngineModule::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshComponent>::UnregisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_Render_Mobility::UnregisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::WorldRendererSystem>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshComponent>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MeshComponent>::UnregisterType( typeRegistry );
}

