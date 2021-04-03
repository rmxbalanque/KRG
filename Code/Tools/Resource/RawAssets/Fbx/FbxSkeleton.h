#ifdef _WIN32
#pragma once
#include "Tools/Resource/_Module/API.h"
#include "Tools/Resource/RawAssets/RawSkeleton.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fbx 
    {
        class FbxSceneContext;

        //-------------------------------------------------------------------------

        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );

		// Temp HACK
        KRG_TOOLS_RESOURCE_API void ReadSkeleton( FbxSceneContext const& sceneCtx, String const& skeletonRootBoneName, RawAssets::RawSkeleton& skeleton );
    }
}
#endif