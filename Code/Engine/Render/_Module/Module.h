#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "Engine/Render/Renderers/StaticMeshRenderer.h"
#include "Engine/Render/Renderers/SkeletalMeshRenderer.h"
#include "System/Render/DebugRenderer/DebugRenderer.h"
#include "System/Render/ResourceLoaders/MeshLoader.h"
#include "System/Render/ResourceLoaders/ShaderLoader.h"
#include "System/Render/ResourceLoaders/TextureLoader.h"
#include "System/Render/ResourceLoaders/MaterialLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
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

            #if KRG_DEBUG_INSTRUMENTATION
            DebugRenderer                       m_debugRenderer;
            #endif

            bool                                m_initialized = false;
        };
    }
}