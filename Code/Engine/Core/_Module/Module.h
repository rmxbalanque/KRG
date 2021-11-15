#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/ResourceLoaders/ResourceLoader_EntityMap.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Render/RenderDevice.h"
#include "System/Input/InputSystem.h"
#include "System/Render/Imgui/ImguiSystem.h"
#include "System/Resource/ResourceProvider.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Threading/TaskSystem.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::EngineCore
{
    class KRG_ENGINE_CORE_API EngineModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        EngineModule( SettingsRegistry& settingsRegistry );

        virtual bool Initialize( ModuleContext& context ) final;
        virtual void Shutdown( ModuleContext& context ) final;

        inline SystemRegistry* GetSystemRegistry() { return &m_systemRegistry; }
        inline TaskSystem* GetTaskSystem() { return &m_taskSystem; }
        inline TypeSystem::TypeRegistry* GetTypeRegistry() { return &m_typeRegistry; }
        inline Input::InputSystem* GetInputSystem() { return &m_inputSystem; }
        inline Resource::ResourceSystem* GetResourceSystem() { return &m_resourceSystem; }
        inline Render::RenderDevice* GetRenderDevice() { return m_pRenderDevice; }
        inline EntityWorldManager* GetEntityWorldManager() { return &m_entityWorldManager; }

        #if KRG_DEVELOPMENT_TOOLS
        inline ImGuiX::ImguiSystem* GetImguiSystem() { return &m_imguiSystem; }
        void EnableImguiViewports() { m_imguiViewportsEnabled = true; }
        #endif

    private:

        // System
        SettingsRegistry&                               m_settingsRegistry;
        TaskSystem                                      m_taskSystem;
        TypeSystem::TypeRegistry                        m_typeRegistry;
        SystemRegistry                                  m_systemRegistry;
        Input::InputSystem                              m_inputSystem;

        // Resource
        Resource::ResourceProvider*                     m_pResourceProvider = nullptr;
        Resource::ResourceSystem                        m_resourceSystem;

        // Rendering
        Render::RenderDevice*                           m_pRenderDevice = nullptr;

        // ImGui
        #if KRG_DEVELOPMENT_TOOLS
        ImGuiX::ImguiSystem                             m_imguiSystem;
        bool                                            m_imguiViewportsEnabled = false;
        #endif

        // Entity
        EntityWorldManager                              m_entityWorldManager;
        EntityModel::EntityMapLoader                    m_mapLoader;
    };
}