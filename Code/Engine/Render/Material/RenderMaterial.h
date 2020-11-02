#pragma once

#include "../_Module/API.h"
#include "System/Render/RenderShader.h"
#include "System/Render/RenderStates.h"
#include "System/Render/RenderPipelineState.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API Material : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'MTRL' );
        friend class MaterialCompiler;
        friend class MaterialLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_pDiffuseTexture ) );

    public:

        virtual bool IsValid() const override { return true; }

        inline Texture const* GetDiffuseTexture() const { KRG_ASSERT( IsValid() ); return m_pDiffuseTexture.GetPtr(); }

    private:

        TResourcePtr<Texture>   m_pDiffuseTexture;
    };
}