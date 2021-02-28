#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsDebugViewController : public Debug::DebugView
    {
    public:

        PhysicsDebugViewController();

        void Initialize( PhysicsSystem* pPhysicSystem );
        void Shutdown();

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

        void DrawDebugMenu( UpdateContext const& context );
        void DrawPhysXMenu( UpdateContext const& context );
        void DrawSceneMenu( PhysicsScene* pScene );

    private:

        PhysicsSystem*      m_pPhysicsSystem = nullptr;
        float               m_recordingTimeSeconds = 0.5f;
    };
}
#endif