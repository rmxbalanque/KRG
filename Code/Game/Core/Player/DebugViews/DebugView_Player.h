#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

namespace KRG { class PlayerManager; }

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Player
{
    class PlayerDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( PlayerDebugView );

    public:

        PlayerDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityUpdateContext const& context );
        void DrawActionDebuggerWindow( EntityUpdateContext const& context );

    private:

        EntityWorld const*              m_pWorld = nullptr;
        PlayerManager const*            m_pPlayerManager = nullptr;
        bool                            m_isActionDebuggerWindowOpen = false;
    };
}
#endif