#ifdef _WIN32
#pragma once

#include "Tools/Resource/RawAssets/RawMesh.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace gltf
    {
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile = String() );
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( FileSystem::Path const& sourceFilePath, int32 maxBoneInfluences = 4 );
    }
}
#endif