#pragma once

#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    // Create a DDS texture from the supplied texture
    bool ConvertTexture( FileSystem::Path const& texturePath, TextureType type, TVector<Byte>& rawData );
}