#pragma once

#include "API.h"
#include "Engine/Physics/Debug/PhysicsDebugViewController.h"
#include "Engine/Physics/Debug/PhysxDebugRenderer.h"
#include "Engine/Physics/PhysicsWorld.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "System/Physics/ResourceLoaders/PhysicsGeometryLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class KRG_ENGINE_PHYSICS_API EngineModule : public IModule
        {
            KRG_REGISTER_MODULE;

        public:

            virtual bool Initialize( ModuleContext& context ) override final;
            virtual void Shutdown( ModuleContext& context ) override final;

        private:

            PhysicsWorld                m_physicsWorld;
            PhysicsGeometryLoader       m_physicsGeometryLoader;

            #if KRG_DEBUG_INSTRUMENTATION
            PhysicsRenderer             m_physicsRenderer;
            PhysicsDebugViewController  m_physicsDebugViewController;
            #endif

            bool                        m_initialized = false;
        };
    }
}