#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Render/RendererRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        m_navmeshSystem.Initialize();
        context.RegisterResourceLoader( &m_navmeshLoader );
        context.RegisterSystem( m_navmeshSystem );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_navmeshRenderer.Initialize();
        auto pRendererRegistry = context.GetSystem<Render::RendererRegistry>();
        KRG_ASSERT( pRendererRegistry != nullptr );
        pRendererRegistry->RegisterRenderer( &m_navmeshRenderer );
        #endif

        //-------------------------------------------------------------------------

        m_initialized = true;
        return m_initialized;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        if ( m_initialized )
        {
            #if KRG_DEVELOPMENT_TOOLS
            auto pRendererRegistry = context.GetSystem<Render::RendererRegistry>();
            KRG_ASSERT( pRendererRegistry != nullptr );
            pRendererRegistry->UnregisterRenderer( &m_navmeshRenderer );
            m_navmeshRenderer.Shutdown();
            #endif

            //-------------------------------------------------------------------------

            context.UnregisterSystem( m_navmeshSystem );
            context.UnregisterResourceLoader( &m_navmeshLoader );
            m_navmeshSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        m_initialized = false;
    }
}
