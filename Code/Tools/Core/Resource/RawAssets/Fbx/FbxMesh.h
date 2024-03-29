#pragma once

#include "Tools/Core/Resource/RawAssets/RawMesh.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    KRG_TOOLS_CORE_API TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile = String() );
    KRG_TOOLS_CORE_API TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( FileSystem::Path const& sourceFilePath, int32 maxBoneInfluences );
}