#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class CoverManager;

    //-------------------------------------------------------------------------

    class CoverDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( CoverDebugView );

    public:

        CoverDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityWorldUpdateContext const& context );
        void DrawOverviewWindow( EntityWorldUpdateContext const& context );

    private:

        EntityWorld const*              m_pWorld = nullptr;
        CoverManager*                   m_pCoverManager = nullptr;
        bool                            m_isOverviewWindowOpen = false;
    };
}
#endif