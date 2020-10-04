#ifdef _WIN32
#pragma once

#include "../../_Module/API.h"
#include "Tools/Resource/RawAssets/RawAnimation.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace gltf
    {
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawAnimation> ReadAnimation( FileSystemPath const& animationFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& animationName = String() );
    }
}
#endif