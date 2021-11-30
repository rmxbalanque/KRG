#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class PlayerManager;

    //-------------------------------------------------------------------------

    class CameraDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( CameraDebugView );

    public:

        CameraDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityUpdateContext const& context );
        void DrawCameraWindow( EntityUpdateContext const& context );

    private:

        EntityWorld const*              m_pWorld = nullptr;
        PlayerManager const*        m_pPlayerWorldSystem = nullptr;
        bool                            m_isCameraDebugWindowOpen = false;
    };
}
#endif