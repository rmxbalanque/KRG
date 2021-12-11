#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Render/RenderDevice.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "System/Resource/ResourceSystem.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Drawing/DebugDrawing.h"
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

        inline EntityWorldManager* GetEntityWorldManager() { return m_pEntityWorldManager; }
        inline EntityWorldManager const* GetEntityWorldManager() const { return m_pEntityWorldManager; }

        // ResourceLoader Registration
        //-------------------------------------------------------------------------

        inline void RegisterResourceLoader( Resource::ResourceLoader* pLoader ) { return m_pResourceSystem->RegisterResourceLoader( pLoader ); }
        inline void UnregisterResourceLoader( Resource::ResourceLoader* pLoader ) { return m_pResourceSystem->UnregisterResourceLoader( pLoader ); }

        // Engine System Registration
        //-------------------------------------------------------------------------

        template<typename T> inline T* GetSystem() const { return m_pSystemRegistry->GetSystem<T>(); }
        inline void RegisterSystem( ISystem& system ) { return m_pSystemRegistry->RegisterSystem( &system ); }
        inline void UnregisterSystem( ISystem& system ) { return m_pSystemRegistry->UnregisterSystem( &system ); }

    private:

        String                              m_applicationName;
        SettingsRegistry*                   m_pSettingsRegistry = nullptr;
        TaskSystem*                         m_pTaskSystem = nullptr;
        TypeSystem::TypeRegistry*           m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        SystemRegistry*                     m_pSystemRegistry = nullptr;
        EntityWorldManager*                 m_pEntityWorldManager = nullptr;
        Render::RenderDevice*               m_pRenderDevice = nullptr;
    };
}