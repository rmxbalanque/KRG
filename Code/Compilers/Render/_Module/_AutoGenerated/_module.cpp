//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Compilers\Render\_Module\Module.h"
#include "System/Resource/ResourceSystem.h"
#include "D:\Kruger\Code\Compilers\Render\_Module\_AutoGenerated\1650622419.cpp"
#include "D:\Kruger\Code\Compilers\Render\_Module\_AutoGenerated\1739523965.cpp"
#include "D:\Kruger\Code\Compilers\Render\_Module\_AutoGenerated\1868817221.cpp"
#include "D:\Kruger\Code\Compilers\Render\_Module\_AutoGenerated\1906592193.cpp"
#include "D:\Kruger\Code\Compilers\Render\_Module\_AutoGenerated\2261760404.cpp"

//-------------------------------------------------------------------------

void KRG::Render::ResourceCompilerModule::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MaterialResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::TextureResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::ShaderResourceDescriptor>::RegisterType( typeRegistry );
}

void KRG::Render::ResourceCompilerModule::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::ShaderResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::TextureResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MaterialResourceDescriptor>::UnregisterType( typeRegistry );
}

