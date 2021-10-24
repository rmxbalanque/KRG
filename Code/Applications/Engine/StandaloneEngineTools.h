#pragma once

#include "Engine/Core/DevelopmentUI.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EntityWorldManager;
    class EntityWorldDebugger;

    //-------------------------------------------------------------------------

    class StandaloneEngineTools final : public ImGuiX::DevelopmentUI
    {
        struct ModalPopupMessage
        {
            String                                          m_ID;
            String                                          m_channel;
            String                                          m_message;
            Log::Severity                                   m_severity;
            bool                                            m_isOpen = true;
        };

    public:

        virtual void Initialize( UpdateContext const& context ) override final;
        virtual void Shutdown( UpdateContext const& context ) override final;
        virtual void Update( UpdateContext const& context, Render::ViewportManager & viewportSystem ) override final;

    private:

        void DrawPopups( UpdateContext const& context );
        void DrawOverlayMenu( UpdateContext const& context );
        void DrawDebugWindows( UpdateContext const& context );
        void DrawOverlayStatusBar( UpdateContext const& context );

    protected:

        EntityWorldManager*                                 m_pWorldManager = nullptr;
        EntityWorldDebugger*                                m_pWorldDebugger = nullptr;

        TVector<ModalPopupMessage>                          m_modalPopups;
        Seconds                                             m_avgTimeDelta = 0.0f;
        bool                                                m_debugOverlayEnabled = false;
    };
}
#endif