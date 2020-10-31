#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysicsSystem.h"
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

            void Initialize( PhysicsSystem* pPhysicSystem );
            void Shutdown( PhysicsSystem* pPhysicSystem );

        private:

            virtual void DrawWindows( UpdateContext const& context ) override final;
            void DrawDebugMenu( UpdateContext const& context );

        private:

            PhysicsSystem*      m_pPhysicsWorld = nullptr;
            float               m_recordingTimeSeconds = 0.5f;
        };
    }
}
#endif