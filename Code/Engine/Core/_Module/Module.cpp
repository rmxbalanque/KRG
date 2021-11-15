#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Resource/ResourceProviders/NetworkResourceProvider.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Render/RenderSettings.h"
#include "System/Network/NetworkSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EngineCore
{
    EngineModule::EngineModule( SettingsRegistry& settingsRegistry )
        : m_settingsRegistry( settingsRegistry )
        , m_resourceSystem( m_taskSystem )
    {}

    bool EngineModule::Initialize( ModuleContext& context )
    {
        Resource::Settings const* pResourceSettings = m_settingsRegistry.GetSettings<Resource::Settings>();
        KRG_ASSERT( pResourceSettings != nullptr );

        Render::Settings const* pRenderSettings = m_settingsRegistry.GetSettings<Render::Settings>();
        KRG_ASSERT( pRenderSettings != nullptr );

        //-------------------------------------------------------------------------

        if ( !Network::NetworkSystem::Initialize() )
        {
            KRG_LOG_ERROR( "Render", "Failed to initialize network system" );
            return false;
        }

        #if KRG_DEVELOPMENT_TOOLS
        m_pResourceProvider = KRG::New<Resource::NetworkResourceProvider>( pResourceSettings );
        #endif

        if ( m_pResourceProvider == nullptr )
        {
            KRG_LOG_ERROR( "Resource", "Failed to create resource provider" );
            return false;
        }

        if ( !m_pResourceProvider->Initialize() )
        {
            KRG_LOG_ERROR( "Resource", "Failed to intialize resource provider" );
            KRG::Delete( m_pResourceProvider );
            return false;
        }

        m_pRenderDevice = KRG::New<Render::RenderDevice>();
        if ( !m_pRenderDevice->Initialize( *pRenderSettings ) )
        {
            KRG_LOG_ERROR( "Render", "Failed to create render device" );
            KRG::Delete( m_pRenderDevice );
            return false;
        }

        // Initialization
        //-------------------------------------------------------------------------

        m_taskSystem.Initialize();
        m_resourceSystem.Initialize( m_pResourceProvider );
        m_inputSystem.Initialize();

        #if KRG_DEVELOPMENT_TOOLS
        m_imguiSystem.Initialize( context.GetApplicationName() + ".imgui.ini", m_pRenderDevice, m_imguiViewportsEnabled );
        #endif

        m_mapLoader.SetTypeRegistry( &m_typeRegistry );

        // Register systems
        //-------------------------------------------------------------------------

        m_systemRegistry.RegisterSystem( &m_settingsRegistry );
        m_systemRegistry.RegisterSystem( &m_typeRegistry );
        m_systemRegistry.RegisterSystem( &m_taskSystem );
        m_systemRegistry.RegisterSystem( &m_resourceSystem );
        m_systemRegistry.RegisterSystem( &m_inputSystem );
        m_systemRegistry.RegisterSystem( &m_entityWorldManager );

        // Register Misc
        //-------------------------------------------------------------------------

        m_resourceSystem.RegisterResourceLoader( &m_mapLoader );

        //-------------------------------------------------------------------------

        return true;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        bool wasFullyInitialized = m_pResourceProvider != nullptr && m_pRenderDevice != nullptr;

        //-------------------------------------------------------------------------

        if ( wasFullyInitialized )
        {
            // Unregister Misc
            //-------------------------------------------------------------------------

            m_resourceSystem.UnregisterResourceLoader( &m_mapLoader );

            // Unregister systems
            //-------------------------------------------------------------------------

            m_systemRegistry.UnregisterSystem( &m_entityWorldManager );
            m_systemRegistry.UnregisterSystem( &m_inputSystem );
            m_systemRegistry.UnregisterSystem( &m_resourceSystem );
            m_systemRegistry.UnregisterSystem( &m_taskSystem );
            m_systemRegistry.UnregisterSystem( &m_typeRegistry );
            m_systemRegistry.UnregisterSystem( &m_settingsRegistry );

            // Shutdown
            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            m_imguiSystem.Shutdown();
            #endif

            m_inputSystem.Shutdown();
            m_resourceSystem.Shutdown();
            m_taskSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        if ( m_pRenderDevice != nullptr )
        {
            m_pRenderDevice->Shutdown();
            KRG::Delete( m_pRenderDevice );
        }

        if ( m_pResourceProvider != nullptr )
        {
            m_pResourceProvider->Shutdown();
            KRG::Delete( m_pResourceProvider );
        }

        Network::NetworkSystem::Shutdown();
    }
}