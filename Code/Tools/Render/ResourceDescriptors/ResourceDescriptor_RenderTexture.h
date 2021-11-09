#pragma once

#include "Tools/Render/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Render/RenderTexture.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    enum class TextureType
    {
        KRG_REGISTER_ENUM

        Default,
        TangentSpaceNormals,
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_RENDER_API TextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( TextureResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const { return Texture::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath     m_path;
        KRG_EXPOSE TextureType      m_type = TextureType::Default;
        KRG_EXPOSE String           m_name; // Optional property needed for extracting textures out of container files (e.g. glb, fbx)
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_RENDER_API CubemapTextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( CubemapTextureResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const { return CubemapTexture::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath     m_path;
    };
}