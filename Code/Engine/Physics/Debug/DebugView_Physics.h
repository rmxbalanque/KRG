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
    class PhysicsShapeComponent;
    class PhysicsStateController;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( PhysicsDebugView );

    public:

        static bool DrawMaterialDatabaseView( UpdateContext const& context );

    public:

        PhysicsDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawPhysicsMenu( EntityWorldUpdateContext const& context );
        void DrawPVDMenu( EntityWorldUpdateContext const& context );
        void DrawComponentsWindow( EntityWorldUpdateContext const& context );
        void DrawMaterialDatabaseWindow( EntityWorldUpdateContext const& context );

        void DrawComponentVisualization( EntityWorldUpdateContext const& context, PhysicsShapeComponent const* pComponent ) const;

    private:

        PhysicsSystem*          m_pPhysicsSystem = nullptr;
        PhysicsWorldSystem*     m_pPhysicsWorldSystem = nullptr;
        void*                   m_pSelectedComponent = nullptr;
        float                   m_recordingTimeSeconds = 0.5f;
        bool                    m_isComponentWindowOpen = false;
        bool                    m_isMaterialDatabaseWindowOpen = false;
    };
}
#endif