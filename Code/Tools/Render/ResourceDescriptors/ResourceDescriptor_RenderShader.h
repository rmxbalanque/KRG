#pragma once

#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    enum class ShaderType : uint8
    {
        KRG_REGISTER_ENUM

        Vertex = 0,
        Geometry,
        Pixel,
        Hull,
        Compute,
    };

    struct ShaderResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( ShaderResourceDescriptor );

        KRG_EXPOSE ShaderType           m_shaderType = ShaderType::Vertex;
        KRG_EXPOSE ResourcePath         m_shaderPath;
    };
}