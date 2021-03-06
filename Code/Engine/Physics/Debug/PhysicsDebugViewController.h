#pragma once

#include "../_Module/API.h"
#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsWorldSystem;
    class PhysicsScene;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsDebugViewController : public Debug::DebugView
    {
    public:

        PhysicsDebugViewController();

        void Initialize( PhysicsSystem* pPhysicSystem, PhysicsWorldSystem* pPhysicsWorldSystem );
        void Shutdown();

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

        void DrawDebugMenu( UpdateContext const& context );
        void DrawSceneMenu( PhysicsScene* pScene );
        void DrawComponentsWindow( UpdateContext const& context );

    private:

        PhysicsSystem*          m_pPhysicsSystem = nullptr;
        PhysicsWorldSystem*     m_pPhysicsWorldSystem = nullptr;
        float                   m_recordingTimeSeconds = 0.5f;
        bool                    m_isComponentWindowOpen = false;
    };
}
#endif