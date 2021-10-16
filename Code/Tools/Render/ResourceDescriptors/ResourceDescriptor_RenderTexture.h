#pragma once

#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct TextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( TextureResourceDescriptor );

        KRG_EXPOSE ResourcePath     m_texturePath;
    };
}