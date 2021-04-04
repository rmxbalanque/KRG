#pragma once

#include "RenderingSystem.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Update/UpdateContext.h"

#include "Engine/Core/_Module/Module.h"
#include "Engine/Physics/_Module/Module.h"
#include "Engine/Render/_Module/Module.h"
#include "Engine/Camera/_Module/Module.h"
#include "Engine/Animation/_Module/Module.h"
#include "Engine/Navmesh/_Module/Module.h"
#include "Game/Core/_Module/Module.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Engine
    {
        friend class EngineApplication;
        friend class EditorApplication;

        //-------------------------------------------------------------------------

        class EngineUpdateContext : public UpdateContext
        {
            friend Engine;
        };

    public:

        Engine( TFunction<bool( KRG::String const& error )>&& errorHandler );

        bool Initialize( String const& applicationName, Int2 const& windowDimensions );
        bool Shutdown();
        bool Update();

    protected:

        void UpdateMainWindowSize( Int2 const& windowDimensions );

        virtual void RegisterTypes();
        virtual void UnregisterTypes();

        bool InitializeModules();
        void LoadModuleResources( Resource::ResourceSystem& resourceSystem );
        bool OnModuleResourceLoadingComplete();
        void UnloadModuleResources( Resource::ResourceSystem& resourceSystem );
        void ShutdownModules();

    protected:

        TFunction<bool( KRG::String const& error )>     m_fatalErrorHandler;
        SettingsRegistry                                m_settingsRegistry;

        // Modules
        //-------------------------------------------------------------------------

        TVector<IModule*>                               m_modules;

        EngineCore::EngineModule                        m_module_engine_core;
        Physics::EngineModule                           m_module_engine_physics;
        Render::EngineModule                            m_module_engine_render;
        Camera::EngineModule                            m_module_engine_camera;
        Animation::EngineModule                         m_module_engine_animation;
        Navmesh::EngineModule                           m_module_engine_navmesh;
        Game::GameModule                                m_module_game_core;

        // Contexts
        //-------------------------------------------------------------------------

        ModuleContext                                   m_moduleContext;
        EngineUpdateContext                             m_updateContext;

        // Core Engine Systems
        //-------------------------------------------------------------------------

        SystemRegistry*                                 m_pSystemRegistry = nullptr;
        TaskSystem*                                     m_pTaskSystem = nullptr;
        TypeSystem::TypeRegistry*                       m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*                       m_pResourceSystem = nullptr;
        Render::RenderDevice*                           m_pRenderDevice = nullptr;
        Render::ViewportManager*                        m_pViewportManager = nullptr;
        Render::RendererRegistry*                       m_pRendererRegistry = nullptr;
        Render::RenderingSystem                         m_renderingSystem;
        EntityWorld*                                    m_pEntityWorld = nullptr;
        ImGuiX::ImguiSystem*                            m_pImguiSystem = nullptr;
        Input::InputSystem*                             m_pInputSystem = nullptr;
        Navmesh::NavmeshSystem*                         m_pNavmeshSystem = nullptr;
        Physics::PhysicsSystem*                         m_pPhysicsSystem = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        Debug::DebugTools                               m_debugTools;
        ImGuiX::DevelopmentTools*                       m_pDevelopmentTools = nullptr;
        Debug::DrawingSystem*                           m_pDebugDrawingSystem = nullptr;
        #endif

        // Application data
        //-------------------------------------------------------------------------

        DataPath                                        m_startupMap;
        bool                                            m_moduleInitStageReached = false;
        bool                                            m_moduleResourcesInitStageReached = false;
        bool                                            m_finalInitStageReached = false;
        bool                                            m_initialized = false;

        bool                                            m_exitRequested = false;
    };
}