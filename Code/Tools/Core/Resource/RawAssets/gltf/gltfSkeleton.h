#pragma once

#include "Tools/Core/Resource/RawAssets/RawSkeleton.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::gltf
{
    KRG_TOOLS_CORE_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );
}