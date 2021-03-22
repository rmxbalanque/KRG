#include "Engine.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityTypeHelpers.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Engine::Engine( TFunction<bool( KRG::String const& error )>&& errorHandler )
        : m_fatalErrorHandler( errorHandler )
        , m_module_engine_core( m_settingsRegistry )
    {}

    //-------------------------------------------------------------------------

    bool Engine::InitializeModules()
    {
        // !!! Order is important !!!
        //-------------------------------------------------------------------------

        // CORE MODULE IS SPECIAL

        if ( !m_module_engine_physics.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        if ( !m_module_engine_render.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        if ( !m_module_engine_camera.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        if ( !m_module_engine_animation.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        if ( !m_module_engine_navmesh.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        //-------------------------------------------------------------------------

        if ( !m_module_game_core.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize module" );
        }

        // Fill module array
        //-------------------------------------------------------------------------

        m_modules.emplace_back( &m_module_engine_core );
        m_modules.emplace_back( &m_module_engine_physics );
        m_modules.emplace_back( &m_module_engine_render );
        m_modules.emplace_back( &m_module_engine_animation );
        m_modules.emplace_back( &m_module_engine_navmesh );
        m_modules.emplace_back( &m_module_game_core );

        return true;
    }

    void Engine::LoadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        for ( auto pModule : m_modules )
        {
            pModule->LoadModuleResources( resourceSystem );
        }
    }

    bool Engine::OnModuleResourceLoadingComplete()
    {
        for ( auto pModule : m_modules )
        {
            if ( !pModule->OnEngineResourceLoadingComplete() )
            {
                return false;
            }
        }

        return true;
    }

    void Engine::UnloadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        for ( auto pModule : m_modules )
        {
            pModule->UnloadModuleResources( resourceSystem );
        }
    }

    void Engine::ShutdownModules()
    {
        // !!! Order is important !!!
        //-------------------------------------------------------------------------

        m_module_game_core.Shutdown( m_moduleContext );

        //-------------------------------------------------------------------------

        m_module_engine_navmesh.Shutdown( m_moduleContext );
        m_module_engine_animation.Shutdown( m_moduleContext );
        m_module_engine_camera.Shutdown( m_moduleContext );
        m_module_engine_render.Shutdown( m_moduleContext );
        m_module_engine_physics.Shutdown( m_moduleContext );

        // Core module is special

        //-------------------------------------------------------------------------

        m_modules.clear();
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Engine::InitializeDevelopmentUI()
    {
        m_debugUI.Initialize( m_settingsRegistry );
        m_pDevelopmentUI = &m_debugUI;
    }

    void Engine::ShutdownDevelopmentUI()
    {
        m_debugUI.Shutdown();
    }
    #endif

    //-------------------------------------------------------------------------

    bool Engine::Initialize( String const& applicationName, Int2 const& windowDimensions )
    {
        KRG_LOG_MESSAGE( "System", "Engine Application Startup" );

        m_moduleContext.m_applicationName = applicationName;

        #if KRG_DEVELOPMENT_TOOLS
        m_moduleContext.m_pDebugUI = &m_debugUI;
        #endif

        // Initialize Core
        //-------------------------------------------------------------------------

        if ( !m_module_engine_core.Initialize( m_moduleContext ) )
        {
            return m_fatalErrorHandler( "Failed to initialize engine core" );
        }

        m_pTaskSystem = m_module_engine_core.GetTaskSystem();
        m_pTypeRegistry = m_module_engine_core.GetTypeRegistry();
        m_pSystemRegistry = m_module_engine_core.GetSystemRegistry();
        m_pInputSystem = m_module_engine_core.GetInputSystem();
        m_pResourceSystem = m_module_engine_core.GetResourceSystem();
        m_pRenderDevice = m_module_engine_core.GetRenderDevice();
        m_pRendererRegistry = m_module_engine_core.GetRendererRegistry();
        m_pImguiSystem = m_module_engine_core.GetImguiSystem();
        m_pRenderViewportSystem = m_module_engine_core.GetRenderViewportSystem();
        m_pEntityWorld = m_module_engine_core.GetEntityWorld();

        #if KRG_DEVELOPMENT_TOOLS
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

        if ( !InitializeModules() )
        {
            return m_fatalErrorHandler( "Engine Application Startup failed - See EngineApplication log for details" );
        }

        m_pPhysicsSystem = m_pSystemRegistry->GetSystem<Physics::PhysicsSystem>();
        m_pNavmeshSystem = m_pSystemRegistry->GetSystem<Navmesh::NavmeshSystem>();

        m_updateContext.m_pSystemRegistry = m_pSystemRegistry;

        #if KRG_DEVELOPMENT_TOOLS
        m_updateContext.m_pDebugDrawingSystem = m_pDebugDrawingSystem;
        #endif

        // Load engine resources
        //-------------------------------------------------------------------------

        LoadModuleResources( *m_pResourceSystem );

        while ( m_pResourceSystem->IsBusy() )
        {
            m_pResourceSystem->Update();
        }

        if ( !OnModuleResourceLoadingComplete() )
        {
            return m_fatalErrorHandler( "Failed to load required engine resources - See EngineApplication log for details" );
        }

        // Final initialization
        //-------------------------------------------------------------------------

        m_initialized = true;
        m_pRenderDevice->ResizeRenderTargets( windowDimensions );
        m_renderingSystem.Initialize( m_pRenderViewportSystem, m_pRendererRegistry );
        m_pEntityWorld->Initialize( *m_pSystemRegistry );

        // HACK
        m_pRenderViewportSystem->ResizePrimaryViewport( Math::Rectangle( Float2::Zero, Float2( windowDimensions ) ) );

        #if KRG_DEVELOPMENT_TOOLS
        InitializeDevelopmentUI();
        #endif

        // Load startup map
        //-------------------------------------------------------------------------

        if ( m_startupMap.IsValid() )
        {
            auto const sceneResourceID = KRG::ResourceID( m_startupMap );
            m_pEntityWorld->LoadMap( sceneResourceID );
        }

        return true;
    }

    bool Engine::Shutdown()
    {
        KRG_LOG_MESSAGE( "System", "Engine Application Shutdown Started" );

        m_pTaskSystem->WaitForAll();

        // Shutdown World and runtime state
        //-------------------------------------------------------------------------

        if ( m_initialized )
        {
            #if KRG_DEVELOPMENT_TOOLS
            ShutdownDevelopmentUI();
            #endif

            // Wait for resource/object systems to complete all resource unloading
            m_pEntityWorld->Shutdown();

            while ( m_pEntityWorld->IsBusyLoading() || m_pResourceSystem->IsBusy() )
            {
                m_pResourceSystem->Update();
            }

            m_renderingSystem.Shutdown();
            m_initialized = false;
        }

        // Unload engine resources
        //-------------------------------------------------------------------------

        UnloadModuleResources( *m_pResourceSystem );

        while ( m_pResourceSystem->IsBusy() )
        {
            m_pResourceSystem->Update();
        }

        // Shutdown modules
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_updateContext.m_pDebugDrawingSystem = nullptr;
        #endif

        m_updateContext.m_pSystemRegistry = nullptr;

        ShutdownModules();

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

        m_pTaskSystem = nullptr;
        m_pTypeRegistry = nullptr;
        m_pSystemRegistry = nullptr;
        m_pInputSystem = nullptr;
        m_pResourceSystem = nullptr;
        m_pRenderDevice = nullptr;
        m_pRendererRegistry = nullptr;
        m_pImguiSystem = nullptr;
        m_pRenderViewportSystem = nullptr;
        m_pEntityWorld = nullptr;

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_moduleContext.m_pDebugUI = nullptr;
        #endif

        return true;
    }

    //-------------------------------------------------------------------------

    void Engine::OnWindowResize( Int2 const& windowDimensions )
    {
        KRG_ASSERT( m_initialized );
        m_pRenderDevice->ResizeRenderTargets( windowDimensions );
        m_pRenderViewportSystem->ResizePrimaryViewport( Math::Rectangle( Float2(0, 0), Float2( windowDimensions ) ) );
    }

    bool Engine::Update()
    {
        KRG_ASSERT( m_initialized );

        // Check for fatal errors
        //-------------------------------------------------------------------------

        if ( Log::HasFatalErrorOccurred() )
        {
            return m_fatalErrorHandler( Log::GetFatalError().m_message );
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
                m_pDevelopmentUI->Update( m_updateContext, *m_pRenderViewportSystem );
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
                m_renderingSystem.Update( m_updateContext );
            }

            // Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Physics Update" );
                m_updateContext.m_stage = UpdateStage::Physics;

                //-------------------------------------------------------------------------

                m_pPhysicsSystem->Update( m_updateContext );
                m_pEntityWorld->Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext );
            }

            // Post-Physics
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Post-Physics Update" );
                m_updateContext.m_stage = UpdateStage::PostPhysics;

                //-------------------------------------------------------------------------
                
                m_pNavmeshSystem->Update( m_updateContext );
                m_pEntityWorld->Update( m_updateContext );
                m_renderingSystem.Update( m_updateContext );
            }

            // Frame End
            //-------------------------------------------------------------------------
            {
                KRG_PROFILE_SCOPE_SCENE( "Frame End" );
                m_updateContext.m_stage = UpdateStage::FrameEnd;

                //-------------------------------------------------------------------------

                m_pEntityWorld->Update( m_updateContext );

                #if KRG_DEVELOPMENT_TOOLS
                m_pDevelopmentUI->Update( m_updateContext, *m_pRenderViewportSystem );
                m_pImguiSystem->EndFrame();
                #endif

                m_renderingSystem.Update( m_updateContext );
                m_pInputSystem->ClearFrameState();
                m_pRenderDevice->PresentFrame();
            }
        }

        // Update Time
        //-------------------------------------------------------------------------

        // Ensure we dont get crazy time delta's when we hit breakpoints
        #if !KRG_DEVELOPMENT_TOOLS
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