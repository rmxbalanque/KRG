#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Render
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

        context.RegisterSystem( m_rendererRegistry );

        //-------------------------------------------------------------------------

        if ( m_worldRenderer.Initialize( pRenderDevice ) )
        {
            m_rendererRegistry.RegisterRenderer( &m_worldRenderer );
        }
        else
        {
            return false;
        }

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_debugRenderer.Initialize( pRenderDevice ) )
        {
            m_rendererRegistry.RegisterRenderer( &m_debugRenderer );
        }
        else
        {
            return false;
        }

        if ( m_imguiRenderer.Initialize( pRenderDevice ) )
        {
            m_rendererRegistry.RegisterRenderer( &m_imguiRenderer );
        }
        else
        {
            return false;
        }
        #endif

        m_initialized = true;
        return m_initialized;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        if ( m_initialized )
        {
            #if KRG_DEVELOPMENT_TOOLS
            if ( m_imguiRenderer.IsInitialized() )
            {
                m_rendererRegistry.UnregisterRenderer( &m_imguiRenderer );
            }
            m_imguiRenderer.Shutdown();

            if ( m_debugRenderer.IsInitialized() )
            {
                m_rendererRegistry.UnregisterRenderer( &m_debugRenderer );
            }
            m_debugRenderer.Shutdown();
            #endif

            if ( m_worldRenderer.IsInitialized() )
            {
                m_rendererRegistry.UnregisterRenderer( &m_worldRenderer );
            }
            m_worldRenderer.Shutdown();

            //-------------------------------------------------------------------------

            context.UnregisterSystem( m_rendererRegistry );

            context.UnregisterResourceLoader( &m_meshLoader );
            context.UnregisterResourceLoader( &m_shaderLoader );
            context.UnregisterResourceLoader( &m_textureLoader );
            context.UnregisterResourceLoader( &m_materialLoader );

            m_meshLoader.ClearRenderDevice();
            m_shaderLoader.ClearRenderDevice();
            m_textureLoader.ClearRenderDevice();
        }

        m_initialized = false;
    }
}