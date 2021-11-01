#pragma once

#include "Tools/Render/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct KRG_TOOLS_RENDER_API TextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( TextureResourceDescriptor );

        KRG_EXPOSE ResourcePath     m_path;
        KRG_EXPOSE String           m_name; // Optional property needed for extracting textures out of container files (e.g. glb, fbx)
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_RENDER_API CubemapTextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( CubemapTextureResourceDescriptor );

        KRG_EXPOSE ResourcePath     m_texturePathPositiveX;
        KRG_EXPOSE ResourcePath     m_texturePathNegativeX;
        KRG_EXPOSE ResourcePath     m_texturePathPositiveY;
        KRG_EXPOSE ResourcePath     m_texturePathNegativeY;
        KRG_EXPOSE ResourcePath     m_texturePathPositiveZ;
        KRG_EXPOSE ResourcePath     m_texturePathNegativeZ;
    };
}