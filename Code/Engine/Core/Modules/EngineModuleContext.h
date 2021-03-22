#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/DebugUI/DebugUI.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/Renderers/RendererRegistry.h"
#include "System/Entity/EntityWorld.h"
#include "System/Resource/ResourceSystem.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------
// Module Initialization Context
//-------------------------------------------------------------------------
// This is passed through all modules so that they can register their various systems
// It is owned by the engine core module, which is also responsible for creating all core systems

namespace KRG
{
    class KRG_ENGINE_CORE_API ModuleContext
    {
        friend class Engine;

    public:

        ModuleContext() = default;

        inline String const& GetApplicationName() const { return m_applicationName; }

        // Core Engine Systems
        //-------------------------------------------------------------------------

        inline SettingsRegistry* GetSettingsRegistry() { return m_pSettingsRegistry; }
        inline SettingsRegistry const* GetSettingsRegistry() const { return m_pSettingsRegistry; }

        inline SystemRegistry* GetSystemRegistry() { return m_pSystemRegistry; }
        inline SystemRegistry const* GetSystemRegistry() const { return m_pSystemRegistry; }

        inline TaskSystem* GetTaskSystem() { return m_pTaskSystem; }
        inline TaskSystem const* GetTaskSystem() const { return m_pTaskSystem; }

        inline TypeSystem::TypeRegistry* GetTypeRegistry() { return m_pTypeRegistry; }
        inline TypeSystem::TypeRegistry const* GetTypeRegistry() const { return m_pTypeRegistry; }

        inline Resource::ResourceSystem* GetResourceSystem() { return m_pResourceSystem; }
        inline Resource::ResourceSystem const* GetResourceSystem() const { return m_pResourceSystem; }

        inline Render::RenderDevice* GetRenderDevice() { return m_pRenderDevice; }
        inline Render::RenderDevice const* GetRenderDevice() const { return m_pRenderDevice; }

        inline EntityWorld* GetEntityWorld() { return m_pEntityWorld; }
        inline EntityWorld const* GetEntityWorld() const { return m_pEntityWorld; }

        // ResourceLoader Registration
        //-------------------------------------------------------------------------

        inline void RegisterResourceLoader( Resource::ResourceLoader* pLoader ) { return m_pResourceSystem->RegisterResourceLoader( pLoader ); }
        inline void UnregisterResourceLoader( Resource::ResourceLoader* pLoader ) { return m_pResourceSystem->UnregisterResourceLoader( pLoader ); }

        // Engine System Registration
        //-------------------------------------------------------------------------

        template<typename T> inline T* GetSystem() const { return m_pSystemRegistry->GetSystem<T>(); }
        inline void RegisterSystem( ISystem& system ) { return m_pSystemRegistry->RegisterSystem( &system ); }
        inline void UnregisterSystem( ISystem& system ) { return m_pSystemRegistry->UnregisterSystem( &system ); }

        // Entity System Registration
        //-------------------------------------------------------------------------

        inline void RegisterWorldSystem( IWorldEntitySystem* pSystem ) { m_pEntityWorld->RegisterWorldSystem( pSystem ); }
        inline void UnregisterWorldSystem( IWorldEntitySystem* pSystem ) { m_pEntityWorld->UnregisterWorldSystem( pSystem ); }

        // Renderer Registration
        //-------------------------------------------------------------------------

        inline void RegisterRenderer( Render::IRenderer* pRenderer ) { m_pRendererRegistry->RegisterRenderer( pRenderer ); }
        inline void UnregisterRenderer( Render::IRenderer* pRenderer ) { m_pRendererRegistry->UnregisterRenderer( pRenderer ); }

        // Debug UI System
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline Debug::DebugUI* GetDebugUI() { return m_pDebugUI; }
        inline Debug::DebugUI const* GetDebugUI() const { return m_pDebugUI; }
        inline void RegisterDebugView( Debug::DebugView* pDebugView ) { m_pDebugUI->RegisterDebugView( pDebugView ); }
        inline void UnregisterDebugView( Debug::DebugView* pDebugView ) { m_pDebugUI->UnregisterDebugView( pDebugView ); }
        #endif

    private:

        String                              m_applicationName;
        SettingsRegistry*                   m_pSettingsRegistry = nullptr;
        TaskSystem*                         m_pTaskSystem = nullptr;
        TypeSystem::TypeRegistry*           m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        SystemRegistry*                     m_pSystemRegistry = nullptr;
        EntityWorld*                        m_pEntityWorld = nullptr;
        Render::RenderDevice*               m_pRenderDevice = nullptr;
        Render::RendererRegistry*           m_pRendererRegistry = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        Debug::DebugUI*                     m_pDebugUI = nullptr;
        #endif
    };
}