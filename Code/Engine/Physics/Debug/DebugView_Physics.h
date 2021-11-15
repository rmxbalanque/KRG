#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsWorldSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( PhysicsDebugView );

    public:

        PhysicsDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawPhysicsMenu( EntityUpdateContext const& context );
        void DrawPVDMenu( EntityUpdateContext const& context );
        void DrawComponentsWindow( EntityUpdateContext const& context );
        void DrawMaterialDatabaseWindow( EntityUpdateContext const& context );

    private:

        PhysicsSystem*          m_pPhysicsSystem = nullptr;
        PhysicsWorldSystem*     m_pPhysicsWorldSystem = nullptr;
        float                   m_recordingTimeSeconds = 0.5f;
        bool                    m_isComponentWindowOpen = false;
        bool                    m_isMaterialDatabaseWindowOpen = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsMaterialDatabaseDebugView
    {
    public:

        static bool Draw( UpdateContext const& context );
    };
}
#endif