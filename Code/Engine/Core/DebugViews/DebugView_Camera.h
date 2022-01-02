#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class PlayerManager;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API CameraDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( CameraDebugView );

    public:

        static void DrawDebugCameraOptions( EntityWorld const* pWorld );

    public:

        CameraDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityWorldUpdateContext const& context );
        void DrawCameraWindow( EntityWorldUpdateContext const& context );

    private:

        PlayerManager const*            m_pPlayerManager = nullptr;
        bool                            m_isCameraDebugWindowOpen = false;
    };
}
#endif