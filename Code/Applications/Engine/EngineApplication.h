#pragma once

#include "ApplicationGlobalState.h"
#include "RenderingSystem.h"
#include "Engine/Core/DebugUI/DebugUISystem.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityWorld.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/RendererRegistry.h"
#include "System/Resource/ResourceID.h"
#include "System/Resource/ResourceProvider.h"
#include "System/Resource/ResourceSystem.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Debug/DebugDrawingSystem.h"
#include "System/Core/Update/UpdateContext.h"

// Engine modules
//-------------------------------------------------------------------------

#include "Applications/Shared/_AutoGenerated/EngineTypeRegistration.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EngineApplication
    {
        class EngineUpdateContext : public UpdateContext
        {
            friend EngineApplication;
        };

    public:

        EngineApplication() : m_resourceSystem( m_taskSystem ) {}
        virtual ~EngineApplication() = default;
        virtual int Run( S32 argc, char** argv ) = 0;

    protected:

        virtual bool StartDependencies() { return true; }
        virtual bool Initialize();
        virtual bool Shutdown();

        //-------------------------------------------------------------------------

        // This function will process all commandline settings and load all ini settings. Needs to be the very first thing called from the derived application's run method.
        bool ReadSettings( S32 argc, char** argv );

        // This function will save ini settings. Needs to be the very last thing called from the derived application's run method.
        void WriteSettings();

        //-------------------------------------------------------------------------

        bool InitializeModules();
        void ShutdownModules();

        //-------------------------------------------------------------------------

        void LoadStartupMap();
        bool Update();
        void ResizeMainWindow( Int2 windowDimensions );
        void RequestExit();

        virtual bool FatalError( KRG::String const& error ) = 0;

    protected:

        ApplicationGlobalState                          m_applicationGlobalState;

        // Engine Systems
        //-------------------------------------------------------------------------

        SettingsRegistry                                m_settingsRegistry;
        TaskSystem                                      m_taskSystem;
        SystemRegistry                                  m_systemRegistry;
        TypeSystem::TypeRegistry                        m_typeRegistry;
        Resource::ResourceSystem                        m_resourceSystem;
        Resource::ResourceProvider*                     m_pResourceProvider = nullptr;
        Render::RenderDevice                            m_renderDevice;
        Render::RenderingSystem                         m_renderingSystem;
        ImGuiX::System                                  m_imguiSystem;
        Input::InputSystem                              m_inputSystem;
        CameraSystem                                    m_cameraSystem;
        EntityWorld                                     m_world;

        #if KRG_DEBUG_INSTRUMENTATION
        Debug::DebugUISystem                            m_debugUISystem;
        Debug::DrawingSystem                            m_debugDrawingSystem;
        #endif

        // Contexts
        //-------------------------------------------------------------------------

        ModuleContext                                   m_moduleContext;
        EngineUpdateContext                             m_updateContext;

        // Modules
        //-------------------------------------------------------------------------

        Engine::EngineModule                            m_module_engine_core;
        Physics::EngineModule                           m_module_engine_physics;
        Render::EngineModule                            m_module_engine_render;
        Animation::EngineModule                         m_module_engine_animation;
        Game::GameModule                                m_module_game_core;

        // Application data
        //-------------------------------------------------------------------------

        DataPath                                        m_startupMap;
        bool                                            m_isEditorModeEnabled = false;
        bool                                            m_initialized = false;
        bool                                            m_exitRequested = false;
        bool                                            m_hasFocus = false;
    };
}