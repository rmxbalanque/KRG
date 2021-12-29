#pragma once

#include "API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "Engine/Physics/Debug/PhysicsDebugRenderer.h"
#include "Engine/Physics/ResourceLoaders/ResourceLoader_PhysicsMaterialDatabase.h"
#include "Engine/Physics/ResourceLoaders/ResourceLoader_PhysicsMesh.h"
#include "Engine/Physics/ResourceLoaders/ResourceLoader_PhysicsRagdoll.h"
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
        RagdollLoader                        m_physicsRagdollLoader;
        TResourcePtr<PhysicsMaterialDatabase>       m_pPhysicMaterialDB;

        #if KRG_DEVELOPMENT_TOOLS
        PhysicsRenderer                             m_physicsRenderer;
        #endif
    };
}