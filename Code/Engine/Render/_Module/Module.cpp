#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool EngineModule::Initialize( ModuleContext& context )
        {
            auto pRenderDevice = context.GetRenderDevice();
            KRG_ASSERT( pRenderDevice != nullptr );

            //-------------------------------------------------------------------------

            m_meshLoader.SetRenderDevice( pRenderDevice );
            m_shaderLoader.SetRenderDevice( pRenderDevice );
            m_textureLoader.SetRenderDevice( pRenderDevice );

            context.RegisterResourceLoader( &m_meshLoader );
            context.RegisterResourceLoader( &m_shaderLoader );
            context.RegisterResourceLoader( &m_textureLoader );
            context.RegisterResourceLoader( &m_materialLoader );

            //-------------------------------------------------------------------------
            
            m_staticMeshRenderer.Initialize( pRenderDevice );
            m_skeletalMeshRenderer.Initialize( pRenderDevice );

            context.RegisterRenderer( &m_staticMeshRenderer );
            context.RegisterRenderer( &m_skeletalMeshRenderer );

            #if KRG_DEBUG_INSTRUMENTATION
            m_debugRenderer.Initialize( pRenderDevice, context.GetSystem<Debug::DrawingSystem>() );
            context.RegisterRenderer( &m_debugRenderer );
            #endif

            context.RegisterGlobalSystem( &m_staticMeshRenderer );
            context.RegisterGlobalSystem( &m_skeletalMeshRenderer );

            m_initialized = true;
            return m_initialized;
        }

        void EngineModule::Shutdown( ModuleContext& context )
        {
            if( m_initialized )
            {
                context.UnregisterGlobalSystem( &m_skeletalMeshRenderer );
                context.UnregisterGlobalSystem( &m_staticMeshRenderer );

                //-------------------------------------------------------------------------

                #if KRG_DEBUG_INSTRUMENTATION
                context.UnregisterRenderer( &m_debugRenderer );
                m_debugRenderer.Shutdown();
                #endif

                context.UnregisterRenderer( &m_skeletalMeshRenderer );
                context.UnregisterRenderer( &m_staticMeshRenderer );

                m_skeletalMeshRenderer.Shutdown();
                m_staticMeshRenderer.Shutdown();

                //-------------------------------------------------------------------------

                m_meshLoader.ClearRenderDevice();
                m_shaderLoader.ClearRenderDevice();
                m_textureLoader.ClearRenderDevice();

                context.UnregisterResourceLoader( &m_meshLoader );
                context.UnregisterResourceLoader( &m_shaderLoader );
                context.UnregisterResourceLoader( &m_textureLoader );
                context.UnregisterResourceLoader( &m_materialLoader );
            }

            m_initialized = false;
        }
    }
}