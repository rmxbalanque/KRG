#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Resource/ResourceProviders/NetworkResourceProvider.h"
#include "System/Core/Settings/ConfigSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        static ConfigSettingString     g_resourceServerNetworkAddress( "ResourceServerAddress", "Resource" );
        static ConfigSettingInt        g_resourceServerPort( "ResourceServerPort", "Resource", 5556, 0, 9999 );
    }

    //-------------------------------------------------------------------------

    namespace Engine
    {
        EngineModule::EngineModule( SettingsRegistry& settingsRegistry )
            : m_settingsRegistry( settingsRegistry )
            , m_resourceSystem( m_taskSystem )
        {}

        bool EngineModule::Initialize( ModuleContext& context )
        {
            m_pResourceProvider = KRG::New<Resource::NetworkResourceProvider>( Settings::g_resourceServerNetworkAddress, (U32) Settings::g_resourceServerPort );
            if ( !m_pResourceProvider->Initialize() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to intialize resource provider" );
                KRG::Delete( m_pResourceProvider );
                return false;
            }

            m_pRenderDevice = KRG::New<Render::RenderDevice>();
            if ( !m_pRenderDevice->Initialize() )
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
            m_imguiSystem.Initialize();
            m_imguiRenderer.Initialize( m_pRenderDevice );

            m_mapLoader.SetTypeRegistry( &m_typeRegistry );

            #if KRG_DEVELOPMENT_TOOLS
            m_debugUISystem.Initialize( m_settingsRegistry );
            m_cameraDebugViewController.Initialize( &m_cameraSystem );
            m_entityDebugViewController.Initialize( &m_entityWorld );
            m_resourceDebugViewController.Initialize( &m_resourceSystem );
            #endif

            // Register systems
            //-------------------------------------------------------------------------

            m_systemRegistry.RegisterSystem( &m_settingsRegistry );
            m_systemRegistry.RegisterSystem( &m_typeRegistry );
            m_systemRegistry.RegisterSystem( &m_taskSystem );
            m_systemRegistry.RegisterSystem( &m_resourceSystem );
            m_systemRegistry.RegisterSystem( &m_inputSystem );
            m_systemRegistry.RegisterSystem( &m_cameraSystem );
            m_systemRegistry.RegisterSystem( &m_entityWorld );

            #if KRG_DEVELOPMENT_TOOLS
            m_systemRegistry.RegisterSystem( &m_debugUISystem );
            m_systemRegistry.RegisterSystem( &m_debugDrawingSystem );
            #endif

            // Register debug views
            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            m_debugUISystem.RegisterDebugView( &m_systemDebugViewController );
            m_debugUISystem.RegisterDebugView( &m_resourceDebugViewController );
            m_debugUISystem.RegisterDebugView( &m_entityDebugViewController );
            m_debugUISystem.RegisterDebugView( &m_inputDebugViewController );
            m_debugUISystem.RegisterDebugView( &m_cameraDebugViewController );
            #endif

            // Register Misc
            //-------------------------------------------------------------------------

            m_resourceSystem.RegisterResourceLoader( &m_mapLoader );
            m_rendererRegistry.RegisterRenderer( &m_imguiRenderer );

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

                m_rendererRegistry.UnregisterRenderer( &m_imguiRenderer );
                m_resourceSystem.UnregisterResourceLoader( &m_mapLoader );

                // Unregister debug views
                //-------------------------------------------------------------------------

                #if KRG_DEVELOPMENT_TOOLS
                m_debugUISystem.UnregisterDebugView( &m_cameraDebugViewController );
                m_debugUISystem.UnregisterDebugView( &m_inputDebugViewController );
                m_debugUISystem.UnregisterDebugView( &m_entityDebugViewController );
                m_debugUISystem.UnregisterDebugView( &m_resourceDebugViewController );
                m_debugUISystem.UnregisterDebugView( &m_systemDebugViewController );
                #endif

                // Unregister systems
                //-------------------------------------------------------------------------

                #if KRG_DEVELOPMENT_TOOLS
                m_systemRegistry.UnregisterSystem( &m_debugDrawingSystem );
                m_systemRegistry.UnregisterSystem( &m_debugUISystem );
                #endif

                m_systemRegistry.UnregisterSystem( &m_entityWorld );
                m_systemRegistry.UnregisterSystem( &m_cameraSystem );
                m_systemRegistry.UnregisterSystem( &m_inputSystem );
                m_systemRegistry.UnregisterSystem( &m_resourceSystem );
                m_systemRegistry.UnregisterSystem( &m_taskSystem );
                m_systemRegistry.UnregisterSystem( &m_typeRegistry );
                m_systemRegistry.UnregisterSystem( &m_settingsRegistry );

                // Shutdown
                //-------------------------------------------------------------------------

                #if KRG_DEVELOPMENT_TOOLS
                m_resourceDebugViewController.Shutdown();
                m_entityDebugViewController.Shutdown();
                m_cameraDebugViewController.Shutdown();
                m_debugUISystem.Shutdown();
                #endif

                m_imguiRenderer.Shutdown();
                m_imguiSystem.Shutdown();
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
        }
    }
}