#pragma once

#include "API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsWorldSystem.h"
#include "Engine/Physics/Debug/PhysicsDebugViewController.h"
#include "Engine/Physics/Debug/PhysxDebugRenderer.h"
#include "Engine/Physics/ResourceLoaders/PhysicsMeshLoader.h"
#include "Engine/Physics/ResourceLoaders/PhysicsMaterialLoader.h"
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

    private:

        PhysicsSystem               m_physicsSystem;
        PhysicsMeshLoader           m_physicsMeshLoader;
        PhysicsMaterialLoader       m_physicsMaterialLoader;
        PhysicsWorldSystem*         m_pPhysicsWorldSystem = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        PhysicsRenderer             m_physicsRenderer;
        PhysicsDebugViewController  m_physicsDebugViewController;
        #endif
    };
}