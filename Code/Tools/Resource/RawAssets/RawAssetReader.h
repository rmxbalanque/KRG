#ifdef _WIN32
#pragma once

#include "../_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/Pointers.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        class RawMesh;
        class RawSkeleton;
        class RawAnimation;

        //-------------------------------------------------------------------------

        struct ReaderContext
        {
            inline bool IsValid() const{ return m_warningDelegate != nullptr && m_errorDelegate != nullptr; }

            TFunction<void( char const* )>  m_warningDelegate;
            TFunction<void( char const* )>  m_errorDelegate;
        };

        //-------------------------------------------------------------------------

        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, String const& nameOfMeshToCompile = String() );
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, int32 maxBoneInfluences = 4 );

        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, String const& skeletonRootBoneName = String() );
        KRG_TOOLS_RESOURCE_API TUniquePtr<RawAssets::RawAnimation> ReadAnimation( ReaderContext const& ctx, FileSystemPath const& sourceFilePath, RawAssets::RawSkeleton const& rawSkeleton, String const& animationName = String() );
    }
}

#endif