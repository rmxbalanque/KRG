#pragma once

#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Render/Material/RenderMaterial.h"
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
            return m_albedoTexture.GetResourceID().IsValid();
        }

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return Material::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE TResourcePtr<Texture>       m_albedoTexture;
        KRG_EXPOSE TResourcePtr<Texture>       m_metalnessTexture;
        KRG_EXPOSE TResourcePtr<Texture>       m_roughnessTexture;
        KRG_EXPOSE TResourcePtr<Texture>       m_normalMapTexture;
        KRG_EXPOSE TResourcePtr<Texture>       m_aoTexture;
        KRG_EXPOSE Color                       m_albedo = Colors::Black;
        KRG_EXPOSE float                       m_metalness = 0.0f;
        KRG_EXPOSE float                       m_roughness = 1.0f;
        KRG_EXPOSE float                       m_normalScaler = 1.0f;
    };
}