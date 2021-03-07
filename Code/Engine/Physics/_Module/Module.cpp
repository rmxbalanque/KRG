#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Physics/PhysicsMaterialDatabase.h"
#include "System/Core/Settings/ConfigSettings.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    namespace Settings
    {
        static ConfigSettingString const g_physicalMaterialDataPath( "PhysicalMaterialDatabasePath", "Physics", "" );
    }

    //-------------------------------------------------------------------------

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

        if ( !DataPath::IsValidDataPath( Settings::g_physicalMaterialDataPath ) )
        {
            KRG_LOG_ERROR( "Physics", "Invalid physics material database path set: %s", m_pPhysicMaterialDB.GetResourceID().c_str() );
            return false;
        }
        else
        {
            m_pPhysicMaterialDB = ResourceID( Settings::g_physicalMaterialDataPath );
        }

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