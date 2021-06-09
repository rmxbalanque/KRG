#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Resource/RawAssets/RawAnimation.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fbx
    {
        KRG_TOOLS_CORE_API TUniquePtr<RawAssets::RawAnimation> ReadAnimation( FileSystem::Path const& animationFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& animationName );
    }
}