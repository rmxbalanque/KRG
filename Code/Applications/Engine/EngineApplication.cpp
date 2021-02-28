#include "EngineApplication.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityTypeHelpers.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"
#include <string>

//-------------------------------------------------------------------------

// TEMP
#include "Hack_Test.h"

//-------------------------------------------------------------------------

namespace KRG
{
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
            m_pEntityWorld->LoadMap( sceneResourceID );
        }
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::InitializeModules()
    {
        // !!! Order is important !!!
        //-------------------------------------------------------------------------

        // CORE MODULE IS SPECIAL

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

        if ( !m_module_engine_navmesh.Initialize( m_moduleContext ) )
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

        m_module_engine_navmesh.Shutdown( m_moduleContext );
        m_module_engine_animation.Shutdown( m_moduleContext );
        m_module_engine_render.Shutdown( m_moduleContext );
        m_module_engine_physics.Shutdown( m_moduleContext );

        // Core module is special
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::Initialize()
    {
        KRG_LOG_MESSAGE( "System", "Engine Application Startup" );

        // Initialize Core
        //-------------------------------------------------------------------------

        if ( !m_module_engine_core.Initialize( m_moduleContext ) )
        {
            return FatalError( "Failed to initialize engine core" );
        }

        m_pTaskSystem = m_module_engine_core.GetTaskSystem();
        m_pTypeRegistry = m_module_engine_core.GetTypeRegistry();
        m_pSystemRegistry = m_module_engine_core.GetSystemRegistry();
        m_pInputSystem = m_module_engine_core.GetInputSystem();
        m_pResourceSystem = m_module_engine_core.GetResourceSystem();
        m_pRenderDevice = m_module_engine_core.GetRenderDevice();
        m_pRendererRegistry = m_module_engine_core.GetRendererRegistry();
        m_pImguiSystem = m_module_engine_core.GetImguiSystem();
        m_pEntityWorld = m_module_engine_core.GetEntityWorld();
        m_pCameraSystem = m_module_engine_core.GetCameraSystem();

        #if KRG_DEVELOPMENT_TOOLS
        m_pDebugUISystem = m_module_engine_core.GetDebugUISystem();
        m_pDebugDrawingSystem = m_module_engine_core.GetDebugDrawingSystem();
        #endif

        // Register Types
        //-------------------------------------------------------------------------

        AutoGenerated::RegisterTypes( *m_pTypeRegistry );

        // Modules
        //-------------------------------------------------------------------------

        m_moduleContext.m_pSettingsRegistry = &m_settingsRegistry;
        m_moduleContext.m_pTaskSystem = m_pTaskSystem;
        m_moduleContext.m_pTypeRegistry = m_pTypeRegistry;
        m_moduleContext.m_pResourceSystem = m_pResourceSystem;
        m_moduleContext.m_pSystemRegistry = m_pSystemRegistry;
        m_moduleContext.m_pRenderDevice = m_pRenderDevice;
        m_moduleContext.m_pRendererRegistry = m_pRendererRegistry;
        m_moduleContext.m_pEntityWorld = m_pEntityWorld;

        #if KRG_DEVELOPMENT_TOOLS
        m_moduleContext.m_pDebugUISystem = m_pDebugUISystem;
        #endif

        if ( !InitializeModules() )
        {
            return FatalError( "Engine Application Startup failed - See EngineApplication log for details" );
        }

        m_pPhysicsSystem = m_pSystemRegistry->GetSystem<Physics::PhysicsSystem>();
        m_pNavmeshSystem = m_pSystemRegistry->GetSystem<Navmesh::NavmeshSystem>();

        // World and runtime state initialization
        //-------------------------------------------------------------------------

        m_updateContext.m_pSystemRegistry = m_pSystemRegistry;

        #if KRG_DEVELOPMENT_TOOLS
        m_updateContext.m_pDebugDrawingSystem = m_pDebugDrawingSystem;
        #endif

        //-------------------------------------------------------------------------

        m_pCameraSystem->SetViewDimensions( m_pRenderDevice->GetRenderTargetDimensions() );
        m_renderingSystem.Initialize( m_pRendererRegistry );

        m_pEntityWorld->RegisterWorldSystem( m_pCameraSystem );
        m_pEntityWorld->Initialize( *m_pSystemRegistry );

        //-------------------------------------------------------------------------

        m_initialized = true;
        return true;
    }

    bool EngineApplication::Shutdown()
    {
        KRG_LOG_MESSAGE( "System", "Engine Application Shutdown Started" );

        m_pTaskSystem->WaitForAll();

        // Shutdown World and runtime state
        //-------------------------------------------------------------------------

        // Wait for resource/object systems to complete all resource unloading
        m_pEntityWorld->Shutdown();

        while ( m_pEntityWorld->IsBusyLoading() || m_pResourceSystem->IsBusy() )
        {
            m_pResourceSystem->Update();
        }

        m_pEntityWorld->UnregisterWorldSystem( m_pCameraSystem );
        m_renderingSystem.Shutdown();

        #if KRG_DEVELOPMENT_TOOLS
        m_updateContext.m_pDebugDrawingSystem = nullptr;
        #endif

        m_updateContext.m_pSystemRegistry = nullptr;

        // Modules
        //-------------------------------------------------------------------------

        ShutdownModules();

        #if KRG_DEVELOPMENT_TOOLS
        m_moduleContext.m_pDebugUISystem = nullptr;
        #endif

        m_moduleContext.m_pSettingsRegistry = nullptr;
        m_moduleContext.m_pTaskSystem = nullptr;
        m_moduleContext.m_pTypeRegistry = nullptr;
        m_moduleContext.m_pResourceSystem = nullptr;
        m_moduleContext.m_pSystemRegistry = nullptr;
        m_moduleContext.m_pRenderDevice = nullptr;
        m_moduleContext.m_pRendererRegistry = nullptr;
        m_moduleContext.m_pEntityWorld = nullptr;

        // Unregister types
        //-------------------------------------------------------------------------

        AutoGenerated::UnregisterTypes( *m_pTypeRegistry );

        // Shutdown Core
        //-------------------------------------------------------------------------

        m_module_engine_core.Shutdown( m_moduleContext );

        return true;
    }

    //-------------------------------------------------------------------------

    void EngineApplication::ResizeMainWindow( Int2 windowDimensions )
    {
        KRG_ASSERT( m_initialized );
        m_pRenderDevice->ResizeRenderTargets( windowDimensions );
        m_pCameraSystem->SetViewDimensions( windowDimensions );
    }

    bool EngineApplication::Update()
    {
        KRG_ASSERT( m_initialized );

        if ( m_exitRequested )
        {
            return false;
        }

        // Check for fatal errors
        //-------------------------------------------------------------------------

        if ( Log::HasFatalErrorOccurred() )
        {
            return FatalError( Log::GetFatalError().m_message );
        }

        // Perform Frame Update
        //-------------------------------------------------------------------------

        Profiling::StartFrame();

        Milliseconds deltaTime = 0;
        {
            ScopedSystemTimer frameTimer( deltaTime );

            // Streaming / Loading
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_RESOURCE( "Loading/Streaming" );
                m_pResourceSystem->Update();
                m_pEntityWorld->UpdateLoading();
            }

            // Frame Start
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Frame Start" );
                m_updateContext.m_stage = UpdateStage::FrameStart;

                //-------------------------------------------------------------------------

                m_pInputSystem->Update();

                #if KRG_DEVELOPMENT_TOOLS
                m_pImguiSystem->StartFrame( m_updateContext.GetDeltaTime() );
                m_pDebugUISystem->Update( m_updateContext );
                #endif

                m_pEntityWorld->Update( m_updateContext );
            }

            // Pre-Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Pre-Physics Update" );
                m_updateContext.m_stage = UpdateStage::PrePhysics;

                //-------------------------------------------------------------------------

                m_pEntityWorld->Update( m_updateContext );
                m_pCameraSystem->Update(); // Camera entities should have been updated as part of the pre-physics step, so update the viewports
                m_renderingSystem.Update( m_updateContext, m_pCameraSystem->GetActiveViewports() );
            }

            // Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Physics Update" );
                m_updateContext.m_stage = UpdateStage::Physics;

                //-------------------------------------------------------------------------

                m_pPhysicsSystem->Update( m_updateContext );
                m_pEntityWorld->Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext, m_pCameraSystem->GetActiveViewports() );
            }

            // Post-Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Post-Physics Update" );
                m_updateContext.m_stage = UpdateStage::PostPhysics;

                //-------------------------------------------------------------------------
                
                m_pNavmeshSystem->Update( m_updateContext );
                m_pEntityWorld->Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext, m_pCameraSystem->GetActiveViewports() );
            }

            // Frame End
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Frame End" );
                m_updateContext.m_stage = UpdateStage::FrameEnd;

                //-------------------------------------------------------------------------

                m_pEntityWorld->Update( m_updateContext );

                #if KRG_DEVELOPMENT_TOOLS
                m_pDebugUISystem->Update( m_updateContext );
                m_pImguiSystem->EndFrame();
                #endif

                m_renderingSystem.Update( m_updateContext, m_pCameraSystem->GetActiveViewports() );
                m_pInputSystem->ClearFrameState();
                m_pRenderDevice->PresentFrame();
            }
        }

        // Update Time
        //-------------------------------------------------------------------------

        // Ensure we dont get crazy time delta's when we hit breakpoints
        #if !KRG_CONFIGURATION_FINAL
        if ( deltaTime.ToSeconds() > 1.0f )
        {
            deltaTime = m_updateContext.GetDeltaTime(); // Keep last frame delta
        }
        #endif

        m_updateContext.UpdateDeltaTime( deltaTime );
        EngineClock::Update( deltaTime );
        Profiling::EndFrame();

        // Should we exit?
        //-------------------------------------------------------------------------

        return true;
    }
}