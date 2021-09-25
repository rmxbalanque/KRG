#pragma once

#include "API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsWorldSystem.h"
#include "Engine/Physics/PhysicsMaterialDatabase.h"
#include "Engine/Physics/Debug/DebugView_Physics.h"
#include "Engine/Physics/Debug/PhysxDebugRenderer.h"
#include "Engine/Physics/ResourceLoaders/ResourceLoader_PhysicsMaterialDatabase.h"
#include "Engine/Physics/ResourceLoaders/ResourceLoader_PhysicsMesh.h"
#include "Engine/Core/Modules/IEngineModule.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API EngineModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        virtual bool Initialize( ModuleContext& context ) override final;
        virtual void Shutdown( ModuleContext& context ) override final;

        virtual void LoadModuleResources( Resource::ResourceSystem& resourceSystem ) override final;
        virtual bool OnEngineResourceLoadingComplete() override final;
        virtual void UnloadModuleResources( Resource::ResourceSystem& resourceSystem ) override final;

    private:

        PhysicsSystem                               m_physicsSystem;
        PhysicsMeshLoader                           m_physicsMeshLoader;
        PhysicsMaterialDatabaseLoader               m_physicsMaterialLoader;
        PhysicsWorldSystem*                         m_pPhysicsWorldSystem = nullptr;
        TResourcePtr<PhysicsMaterialDatabase>       m_pPhysicMaterialDB;

        #if KRG_DEVELOPMENT_TOOLS
        PhysicsRenderer                             m_physicsRenderer;
        PhysicsDebugViewController                  m_physicsDebugViewController;
        #endif
    };
}