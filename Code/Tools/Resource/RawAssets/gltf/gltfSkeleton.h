#ifdef _WIN32
#pragma once
#include "../../_Module/API.h"
#include "Tools/Resource/RawAssets/RawSkeleton.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace gltf
    {
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );
    }
}
#endif