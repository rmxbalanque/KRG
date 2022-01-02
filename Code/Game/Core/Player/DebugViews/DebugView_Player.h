#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

namespace KRG { class PlayerManager; }

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Player
{
    class PlayerController;

    //-------------------------------------------------------------------------

    class PlayerDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( PlayerDebugView );

    public:

        PlayerDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityWorldUpdateContext const& context );
        void DrawActionDebuggerWindow( EntityWorldUpdateContext const& context, PlayerController const* pController );
        void DrawPhysicsStateDebuggerWindow( EntityWorldUpdateContext const& context, PlayerController const* pController );

        // HACK since we dont have a UI system yet
        virtual void DrawOverlayElements( EntityWorldUpdateContext const& context ) override;

    private:

        EntityWorld const*              m_pWorld = nullptr;
        PlayerManager const*            m_pPlayerManager = nullptr;
        bool                            m_isActionDebuggerWindowOpen = false;
        bool                            m_isPhysicsStateDebuggerWindowOpen = false;
    };
}
#endif