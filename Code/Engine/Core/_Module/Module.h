#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "Engine/Core/Entity/DebugViews/DebugView_EntityWorld.h"
#include "Engine/Core/DebugViews/DebugView_Input.h"
#include "Engine/Core/DebugViews/DebugView_Resource.h"
#include "Engine/Core/DebugViews/DebugView_System.h"
#include "Engine/Core/DebugTools/DebugTools.h"
#include "Engine/Core/Entity/ResourceLoaders/ResourceLoader_EntityMap.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Render/RenderDevice.h"
#include "System/Render/RendererRegistry.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Input/InputSystem.h"
#include "System/Render/Imgui/ImguiSystem.h"
#include "System/Resource/ResourceProvider.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Threading/TaskSystem.h"
#include "System/Core/Systems/SystemRegistry.h"
#include "System/Core/Debug/DebugDrawingSystem.h"

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
        inline Render::RendererRegistry* GetRendererRegistry() { return &m_rendererRegistry; }
        inline Render::ViewportManager* GetViewportManager() { return &m_viewportManager; }
        inline ImGuiX::ImguiSystem* GetImguiSystem() { return &m_imguiSystem; }

        inline EntityWorld* GetEntityWorld() { return &m_entityWorld; }

        #if KRG_DEVELOPMENT_TOOLS
        inline Debug::DrawingSystem* GetDebugDrawingSystem() { return &m_debugDrawingSystem; }
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
        Render::RendererRegistry                        m_rendererRegistry;
        Render::ViewportManager                         m_viewportManager;

        // ImGui
        ImGuiX::ImguiSystem                             m_imguiSystem;

        // Entity
        EntityWorld                                     m_entityWorld;
        EntityModel::EntityMapLoader                    m_mapLoader;

        // Debug
        #if KRG_DEVELOPMENT_TOOLS
        Debug::DrawingSystem                            m_debugDrawingSystem;
        #endif

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        Debug::SystemDebugViewController                m_systemDebugViewController;
        EntityDebugViewController                       m_entityDebugViewController;
        Input::InputDebugViewController                 m_inputDebugViewController;
        Resource::ResourceDebugViewController           m_resourceDebugViewController;
        #endif
    };
}