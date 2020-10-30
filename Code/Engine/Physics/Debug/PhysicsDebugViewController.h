#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysicsWorld.h"
#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Physics
    {
        class KRG_ENGINE_PHYSICS_API PhysicsDebugViewController : public Debug::DebugView
        {
        public:

            PhysicsDebugViewController();

            void Initialize( PhysicsWorld* pPhysicSystem );
            void Shutdown( PhysicsWorld* pPhysicSystem );

        private:

            virtual void DrawWindows( UpdateContext const& context ) override final;
            void DrawDebugMenu( UpdateContext const& context );

        private:

            PhysicsWorld*      m_pPhysicsWorld = nullptr;
            float               m_recordingTimeSeconds = 0.5f;
        };
    }
}
#endif