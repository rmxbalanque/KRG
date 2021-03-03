#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        m_physicsSystem.Initialize();
        m_pPhysicsWorldSystem = KRG::New<PhysicsWorldSystem>( m_physicsSystem );

        //-------------------------------------------------------------------------

        m_physicsMeshLoader.SetPhysics( &m_physicsSystem.GetPxPhysics() );
        context.RegisterResourceLoader( &m_physicsMeshLoader );

        m_physicsMaterialLoader.SetPhysics( &m_physicsSystem.GetPxPhysics() );
        context.RegisterResourceLoader( &m_physicsMaterialLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_physicsSystem );
        context.RegisterWorldSystem( m_pPhysicsWorldSystem );

        #if KRG_DEVELOPMENT_TOOLS
        m_physicsRenderer.Initialize( context.GetRenderDevice(), &m_physicsSystem );
        context.RegisterRenderer( &m_physicsRenderer );

        m_physicsDebugViewController.Initialize( &m_physicsSystem );
        context.RegisterDebugView( &m_physicsDebugViewController );
        #endif

        return true;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        #if KRG_DEVELOPMENT_TOOLS
        context.UnregisterDebugView( &m_physicsDebugViewController );
        m_physicsDebugViewController.Shutdown();

        context.UnregisterRenderer( &m_physicsRenderer );
        m_physicsRenderer.Shutdown();
        #endif

        context.UnregisterWorldSystem( m_pPhysicsWorldSystem );
        context.UnregisterSystem( m_physicsSystem );

        //-------------------------------------------------------------------------

        context.UnregisterResourceLoader( &m_physicsMaterialLoader );
        m_physicsMaterialLoader.ClearPhysics();

        context.UnregisterResourceLoader( &m_physicsMeshLoader );
        m_physicsMeshLoader.ClearPhysics();

        //-------------------------------------------------------------------------
        
        KRG::Delete( m_pPhysicsWorldSystem );
        m_physicsSystem.Shutdown();
    }
}