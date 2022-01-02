#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::AI
{
    class AIManager;

    //-------------------------------------------------------------------------

    class AIDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( AIDebugView );

    public:

        AIDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityWorldUpdateContext const& context );
        void DrawOverviewWindow( EntityWorldUpdateContext const& context );

    private:

        EntityWorld const*              m_pWorld = nullptr;
        AIManager*                      m_pAIManager = nullptr;
        bool                            m_isOverviewWindowOpen = false;
    };
}
#endif