#ifdef _WIN32
#pragma once

#include "Tools/Resource/RawAssets/RawMesh.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fbx
    {
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( FileSystemPath const& sourceFilePath, String const& nameOfMeshToCompile = String() );
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( FileSystemPath const& sourceFilePath, S32 maxBoneInfluences = 4 );
    }
}
#endif