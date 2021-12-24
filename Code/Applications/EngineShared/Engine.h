#pragma once

#include "RenderingSystem.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Core/DevUI/IDevUI.h"
#include "Engine/Physics/PhysicsSettings.h"
#include "System/Render/RenderSettings.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Types/Function.h"
#include "Engine/Core/Update/UpdateContext.h"

#include "Engine/Core/_Module/Module.h"
#include "Engine/Physics/_Module/Module.h"
#include "Engine/Render/_Module/Module.h"
#include "Engine/Animation/_Module/Module.h"
#include "Engine/Navmesh/_Module/Module.h"
#include "Game/Core/_Module/Module.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Engine
    {
        friend class EngineApplication;

        class EngineUpdateContext : public UpdateContext
        {
            friend Engine;
        };

    public:

        Engine( TFunction<bool( KRG::String const& error )>&& errorHandler );
        virtual ~Engine();

        bool Initialize( String const& applicationName, Int2 const& windowDimensions );
        bool Shutdown();
        bool Update();

        // Needed for window processor access
        Render::RenderingSystem* GetRenderingSystem() { return &m_renderingSystem; }
        Input::InputSystem* GetInputSystem() { return m_pInputSystem; }

    protected:

        virtual void RegisterTypes();
        virtual void UnregisterTypes();

        #if KRG_DEVELOPMENT_TOOLS
        virtual void CreateDevelopmentToolsUI() = 0;
        void DestroyDevelopmentToolsUI() { KRG::Delete( m_pDevToolsUI ); }
        #endif

        //-------------------------------------------------------------------------

        bool InitializeModules();
        void LoadModuleResources( Resource::ResourceSystem& resourceSystem );
        bool OnModuleResourceLoadingComplete();
        void UnloadModuleResources( Resource::ResourceSystem& resourceSystem );
        void ShutdownModules();

    protected:

        TFunction<bool( KRG::String const& error )>     m_fatalErrorHandler;

        // Settings
        //-------------------------------------------------------------------------

        SettingsRegistry                                m_settingsRegistry;
        Resource::Settings                              m_resourceSettings;
        Render::Settings                                m_renderSettings;
        Physics::Settings                               m_physicsSettings;

        // Modules
        //-------------------------------------------------------------------------

        TVector<IModule*>                               m_modules;

        EngineCore::EngineModule                        m_module_engine_core;
        Physics::EngineModule                           m_module_engine_physics;
        Render::EngineModule                            m_module_engine_render;
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
        Render::RenderingSystem                         m_renderingSystem;
        EntityWorldManager*                             m_pEntityWorldManager = nullptr;
        Input::InputSystem*                             m_pInputSystem = nullptr;
        Physics::PhysicsSystem*                         m_pPhysicsSystem = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        ImGuiX::ImguiSystem*                            m_pImguiSystem = nullptr;
        ImGuiX::IDevUI*                                 m_pDevToolsUI = nullptr;
        #endif

        // Application data
        //-------------------------------------------------------------------------

        ResourcePath                                    m_startupMap;
        bool                                            m_moduleInitStageReached = false;
        bool                                            m_moduleResourcesInitStageReached = false;
        bool                                            m_finalInitStageReached = false;
        bool                                            m_initialized = false;

        bool                                            m_exitRequested = false;
    };
}