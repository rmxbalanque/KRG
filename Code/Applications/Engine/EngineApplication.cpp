#include "EngineApplication.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityTypeHelpers.h"
#include "System/Physics/PhysicsWorld.h"
#include "System/Resource/ResourceProviders/NetworkResourceProvider.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Settings/ConfigSettings.h"
#include "System/Core/Time/Timers.h"
#include <string>

//-------------------------------------------------------------------------

// TEMP
#include "Hack_Test.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        static ConfigSettingString     g_resourceServerNetworkAddress( "ResourceServerAddress", "Resource" );
        static ConfigSettingInt        g_resourceServerPort( "ResourceServerPort", "Resource", 5556, 0, 9999 );
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::ReadSettings( S32 argc, char** argv )
    {
        // Get command line settings
        //-------------------------------------------------------------------------

        {
            cli::Parser cmdParser( argc, argv );
            cmdParser.set_optional<bool>( "editor", "editor", false, "Start engine in editor Mode." );
            cmdParser.set_optional<std::string>( "map", "map", " ", "The startup map." );

            if ( !cmdParser.run() )
            {
                return FatalError( "Invalid command line arguments!" );
            }

            m_isEditorModeEnabled = cmdParser.get<bool>( "editor" );
            m_startupMap = cmdParser.get<std::string>( "map" ).c_str();
        }

        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        {
            FileSystemPath const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
            if ( !m_settingsRegistry.LoadFromFile( iniPath ) )
            {
                return FatalError( "Failed to read required settings from INI file" );
            }
        }

        return true;
    }

    void EngineApplication::WriteSettings()
    {
        // Nothing to do
    }

    void EngineApplication::RequestExit()
    {
        KRG_ASSERT( !m_exitRequested );
        m_exitRequested = true;
    }

    void EngineApplication::LoadStartupMap()
    {
        KRG_ASSERT( m_initialized );

        if ( m_startupMap.IsValid() )
        {
            auto const sceneResourceID = KRG::ResourceID( m_startupMap );
            m_world.LoadMap( sceneResourceID );
            m_world.LoadMap( KRG::ResourceID( "data://maps/BR_Minimal.map") );
        }
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::InitializeModules()
    {
        // !!! Order is important !!!
        //-------------------------------------------------------------------------

        if ( !m_module_engine_core.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize module" );
        }

        if ( !m_module_engine_physics.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize module" );
        }

        if ( !m_module_engine_render.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize module" );
        }

        if ( !m_module_engine_animation.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize module" );
        }

        //-------------------------------------------------------------------------

        if ( !m_module_game_core.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize module" );
        }

        return true;
    }

    void EngineApplication::ShutdownModules()
    {
        // !!! Order is important !!!
        //-------------------------------------------------------------------------

        m_module_game_core.Shutdown( m_moduleContext );

        //-------------------------------------------------------------------------

        m_module_engine_animation.Shutdown( m_moduleContext );
        m_module_engine_render.Shutdown( m_moduleContext );
        m_module_engine_physics.Shutdown( m_moduleContext );
        m_module_engine_core.Shutdown( m_moduleContext );
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::Initialize()
    {
        KRG_LOG_MESSAGE( "KRG", "Engine Application Startup" );

        // Initialize Systems
        //-------------------------------------------------------------------------

        m_taskSystem.Initialize();

        //-------------------------------------------------------------------------

        m_pResourceProvider = KRG::New<Resource::NetworkResourceProvider>( Settings::g_resourceServerNetworkAddress, (U32) Settings::g_resourceServerPort );
        if ( !m_pResourceProvider->Initialize() )
        {
            return FatalError( "Engine Application Startup failed - See EngineApplication log for details" );
        }

        m_resourceSystem.Initialize( m_pResourceProvider );

        //-------------------------------------------------------------------------

        if ( !m_renderDevice.Initialize() )
        {
            KRG_LOG_ERROR( "Render", "Failed to create render device" );
            return FatalError( "Engine Application Startup failed - See EngineApplication log for details" );
        }

        //-------------------------------------------------------------------------

        Physics::PhysicsWorld::Initialize();
        m_inputSystem.Initialize();
        m_imguiSystem.Initialize();

        #if KRG_DEBUG_INSTRUMENTATION
        m_debugUISystem.Initialize( m_settingsRegistry );
        #endif

        // Register systems
        //-------------------------------------------------------------------------

        m_systemRegistry.RegisterSystem( &m_settingsRegistry );
        m_systemRegistry.RegisterSystem( &m_typeRegistry );
        m_systemRegistry.RegisterSystem( &m_taskSystem );
        m_systemRegistry.RegisterSystem( &m_resourceSystem );
        m_systemRegistry.RegisterSystem( &m_inputSystem );
        m_systemRegistry.RegisterSystem( &m_cameraSystem );

        #if KRG_DEBUG_INSTRUMENTATION
        m_systemRegistry.RegisterSystem( &m_debugUISystem );
        m_systemRegistry.RegisterSystem( &m_debugDrawingSystem );
        #endif

        // Module context
        //-------------------------------------------------------------------------

        m_moduleContext.m_pSettingsRegistry = &m_settingsRegistry;
        m_moduleContext.m_pTaskSystem = &m_taskSystem;
        m_moduleContext.m_pTypeRegistry = &m_typeRegistry;
        m_moduleContext.m_pResourceSystem = &m_resourceSystem;
        m_moduleContext.m_pSystemRegistry = &m_systemRegistry;
        m_moduleContext.m_pRenderDevice = &m_renderDevice;
        m_moduleContext.m_pRendererRegistry = m_renderingSystem.GetRendererRegistry();
        m_moduleContext.m_pWorld = &m_world;
        
        #if KRG_DEBUG_INSTRUMENTATION
        m_moduleContext.m_pDebugUISystem = &m_debugUISystem;
        #endif

        // Modules
        //-------------------------------------------------------------------------

        AutoGenerated::RegisterTypes( m_typeRegistry );

        if ( !InitializeModules() )
        {
            return FatalError( "Engine Application Startup failed - See EngineApplication log for details" );
        }

        // Update Context
        //-------------------------------------------------------------------------

        m_updateContext.m_pSystemRegistry = &m_systemRegistry;

        #if KRG_DEBUG_INSTRUMENTATION
        m_updateContext.m_pDebugDrawingSystem = &m_debugDrawingSystem;
        #endif

        // World and runtime state initialization
        //-------------------------------------------------------------------------

        m_cameraSystem.SetViewDimensions( m_renderDevice.GetRenderTargetDimensions() );
        m_renderingSystem.Initialize();

        m_world.RegisterGlobalSystem( &m_cameraSystem );
        m_world.Initialize( m_systemRegistry );

        //-------------------------------------------------------------------------

        m_initialized = true;
        return true;
    }

    bool EngineApplication::Shutdown()
    {
        KRG_LOG_MESSAGE( "KRG", "Engine Application Shutdown Started" );

        // Shutdown World and runtime state
        //-------------------------------------------------------------------------

        // Wait for resource/object systems to complete all resource unloading
        m_world.Shutdown();

        while ( m_world.IsBusyLoading() || m_resourceSystem.IsBusy() )
        {
            m_resourceSystem.ExecuteTasksSynchronously();
        }

        m_world.UnregisterGlobalSystem( &m_cameraSystem );
        m_renderingSystem.Shutdown();

        // Update Context
        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        m_updateContext.m_pDebugDrawingSystem = nullptr;
        #endif

        m_updateContext.m_pSystemRegistry = nullptr;

        // Modules
        //-------------------------------------------------------------------------

        ShutdownModules();
        AutoGenerated::UnregisterTypes( m_typeRegistry );

        // Module Context
        //-------------------------------------------------------------------------

        m_moduleContext.m_pSettingsRegistry = nullptr;
        m_moduleContext.m_pTaskSystem = nullptr;
        m_moduleContext.m_pTypeRegistry = nullptr;
        m_moduleContext.m_pResourceSystem = nullptr;
        m_moduleContext.m_pSystemRegistry = nullptr;
        m_moduleContext.m_pRenderDevice = nullptr;
        m_moduleContext.m_pRendererRegistry = nullptr;
        m_moduleContext.m_pWorld = nullptr;

        // Unregister Systems
        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        m_systemRegistry.UnregisterSystem( &m_debugDrawingSystem );
        m_systemRegistry.UnregisterSystem( &m_debugUISystem );
        #endif

        m_systemRegistry.UnregisterSystem( &m_cameraSystem );
        m_systemRegistry.UnregisterSystem( &m_inputSystem );
        m_systemRegistry.UnregisterSystem( &m_resourceSystem );
        m_systemRegistry.UnregisterSystem( &m_taskSystem );
        m_systemRegistry.UnregisterSystem( &m_typeRegistry );
        m_systemRegistry.UnregisterSystem( &m_settingsRegistry );

        // Shutdown Systems
        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        m_debugUISystem.Shutdown();
        #endif

        m_imguiSystem.Shutdown();
        m_inputSystem.Shutdown();
        Physics::PhysicsWorld::Shutdown();

        m_renderDevice.Shutdown();

        if ( m_resourceSystem.IsInitialized() )
        {
            m_resourceSystem.Shutdown();
        }

        if ( m_pResourceProvider != nullptr )
        {
            m_pResourceProvider->Shutdown();
            KRG::Delete( m_pResourceProvider );
        }

        m_taskSystem.Shutdown();

        return true;
    }

    //-------------------------------------------------------------------------

    void EngineApplication::ResizeMainWindow( Int2 windowDimensions )
    {
        KRG_ASSERT( m_initialized );
        m_renderDevice.ResizeRenderTargets( windowDimensions );
        m_cameraSystem.SetViewDimensions( windowDimensions );
    }

    bool EngineApplication::Update()
    {
        KRG_ASSERT( m_initialized );

        // Check for fatal errors
        //-------------------------------------------------------------------------

        if ( Log::HasFatalErrorOccurred() )
        {
            return FatalError( Log::GetFatalError().m_message );
        }

        // Perform Frame Update
        //-------------------------------------------------------------------------

        Profiling::StartFrame();
        KRG_PROFILE_SCOPE_COLOR( "Frame", MP_PURPLE );

        Milliseconds timeDeltaMS = 0;
        {
            ScopedTimer frameTimer( timeDeltaMS );

            // Streaming / Loading
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Loading/Streaming" );
                m_resourceSystem.WaitForTasksToComplete();
                m_world.UpdateLoading();
                m_resourceSystem.ExecuteTasks();
            }

            // Frame Start
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Frame Start" );
                m_updateContext.m_stage = UpdateStage::FrameStart;

                //-------------------------------------------------------------------------

                m_inputSystem.Update();

                #if KRG_DEBUG_INSTRUMENTATION
                m_imguiSystem.Update( m_updateContext.GetDeltaTime() );
                m_debugUISystem.Update( m_updateContext );
                #endif

                m_world.Update( m_updateContext );
            }

            // Pre-Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Pre-Physics Update" );
                m_updateContext.m_stage = UpdateStage::PrePhysics;

                //-------------------------------------------------------------------------

                m_world.Update( m_updateContext );
                m_cameraSystem.Update(); // Camera entities should have been updated as part of the pre-physics step, so update the viewports
                m_renderingSystem.Update( m_updateContext, m_cameraSystem.GetActiveViewports() );
            }

            // Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Physics Update" );
                m_updateContext.m_stage = UpdateStage::Physics;

                //-------------------------------------------------------------------------

                Physics::PhysicsWorld::Update( m_updateContext.GetDeltaTime() );
                m_world.Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext, m_cameraSystem.GetActiveViewports() );
            }

            // Post-Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Post-Physics Update" );
                m_updateContext.m_stage = UpdateStage::PostPhysics;

                //-------------------------------------------------------------------------

                m_world.Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext, m_cameraSystem.GetActiveViewports() );
            }

            // Frame End
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE( "Frame End" );
                m_updateContext.m_stage = UpdateStage::FrameEnd;

                //-------------------------------------------------------------------------

                m_world.Update( m_updateContext );

                #if KRG_DEBUG_INSTRUMENTATION
                m_debugUISystem.Update( m_updateContext );
                #endif

                m_renderingSystem.Update( m_updateContext, m_cameraSystem.GetActiveViewports() );
                m_inputSystem.ClearFrameState();
                m_renderDevice.PresentFrame();
            }
        }

        m_updateContext.SetTimeDelta( timeDeltaMS.ToSeconds() );
        Profiling::EndFrame();

        // Should we exit?
        //-------------------------------------------------------------------------

        if ( m_exitRequested )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}