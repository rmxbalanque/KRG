#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Physics/PhysicsSettings.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        m_physicsSystem.Initialize();
        m_pPhysicsWorldSystem = KRG::New<PhysicsWorldSystem>( m_physicsSystem );

        //-------------------------------------------------------------------------

        m_physicsMeshLoader.SetPhysics( &m_physicsSystem );
        context.RegisterResourceLoader( &m_physicsMeshLoader );

        m_physicsMaterialLoader.SetPhysics( &m_physicsSystem );
        context.RegisterResourceLoader( &m_physicsMaterialLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_physicsSystem );
        context.RegisterWorldSystem( m_pPhysicsWorldSystem );

        #if KRG_DEVELOPMENT_TOOLS
        m_physicsRenderer.Initialize( context.GetRenderDevice(), &m_physicsSystem );
        context.RegisterRenderer( &m_physicsRenderer );

        m_physicsDebugViewController.Initialize( &m_physicsSystem, m_pPhysicsWorldSystem );
        context.RegisterDebugView( &m_physicsDebugViewController );
        #endif

        //-------------------------------------------------------------------------

        auto pSettings = context.GetSettingsRegistry()->GetSettings<Settings>();
        KRG_ASSERT( pSettings != nullptr );
        m_pPhysicMaterialDB = ResourceID( pSettings->m_physicalMaterialDataPath );

        //-------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------

    void EngineModule::LoadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        resourceSystem.LoadResource( m_pPhysicMaterialDB );
    }

    bool EngineModule::OnEngineResourceLoadingComplete()
    {
        return m_pPhysicMaterialDB.IsLoaded() && m_pPhysicMaterialDB->IsValid();
    }

    void EngineModule::UnloadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        resourceSystem.UnloadResource( m_pPhysicMaterialDB );
    }
}