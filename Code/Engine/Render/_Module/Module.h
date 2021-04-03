#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "Engine/Render/Renderers/StaticMeshRenderer.h"
#include "Engine/Render/Renderers/SkeletalMeshRenderer.h"
#include "System/Render/Renderers/DebugRenderer/DebugRenderer.h"
#include "Engine/Render/ResourceLoaders/RenderMeshLoader.h"
#include "Engine/Render/ResourceLoaders/RenderShaderLoader.h"
#include "Engine/Render/ResourceLoaders/RenderTextureLoader.h"
#include "Engine/Render/ResourceLoaders/RenderMaterialLoader.h"

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

        StaticMeshRenderer                  m_staticMeshRenderer;
        SkeletalMeshRenderer                m_skeletalMeshRenderer;

        #if KRG_DEVELOPMENT_TOOLS
        DebugRenderer                       m_debugRenderer;
        #endif

        bool                                m_initialized = false;
    };
}