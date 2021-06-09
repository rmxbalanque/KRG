#pragma once
#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Resource/RawAssets/RawSkeleton.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    class FbxSceneContext;

    //-------------------------------------------------------------------------

    KRG_TOOLS_CORE_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );

    // Temp HACK
    KRG_TOOLS_CORE_API void ReadSkeleton( FbxSceneContext const& sceneCtx, String const& skeletonRootBoneName, RawAssets::RawSkeleton& skeleton );
}