#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "System/Render/RenderDevice.h"
#include "Engine/Render/Renderers/MeshRenderer.h"
#include "Engine/Render/Renderers/DebugRenderer.h"
#include "Engine/Render/Renderers/ImguiRenderer.h"
#include "Engine/Render/ResourceLoaders/ResourceLoader_RenderMaterial.h"
#include "Engine/Render/ResourceLoaders/ResourceLoader_RenderMesh.h"
#include "Engine/Render/ResourceLoaders/ResourceLoader_RenderShader.h"
#include "Engine/Render/ResourceLoaders/ResourceLoader_RenderTexture.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API EngineModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        virtual bool Initialize( ModuleContext& context ) override final;
        virtual void Shutdown( ModuleContext& context ) override final;

    private:

        MeshLoader                          m_meshLoader;
        ShaderLoader                        m_shaderLoader;
        TextureLoader                       m_textureLoader;
        MaterialLoader                      m_materialLoader;

        MeshRenderer                        m_meshRenderer;

        #if KRG_DEVELOPMENT_TOOLS
        ImguiRenderer                       m_imguiRenderer;
        DebugRenderer                       m_debugRenderer;
        #endif

        bool                                m_initialized = false;
    };
}