#pragma once

#include "System/Core/Types/Containers.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    // Create a DDS texture from the supplied texture
    bool ConvertTexture( FileSystem::Path const& texturePath, TVector<Byte>& rawData );

    // Create a DDX cubemap from the supplied textures, order is ( +x, -x, +y, -y, +z, -z )
    bool CreateCubemapTexture( TVector<FileSystem::Path> const& texturePaths, TVector<Byte>& rawData );
}