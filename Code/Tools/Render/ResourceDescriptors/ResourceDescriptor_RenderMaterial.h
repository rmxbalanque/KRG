#pragma once

#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct MaterialResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( MaterialResourceDescriptor );

        inline bool IsValid() const
        {
            return m_diffuseTexture.GetResourceID().IsValid();
        }

    public:

        KRG_EXPOSE TResourcePtr<Texture>       m_diffuseTexture;
    };
}