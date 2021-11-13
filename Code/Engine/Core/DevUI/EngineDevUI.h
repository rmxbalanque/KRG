#pragma once

#include "IDevUI.h"
#include "Engine/Core/DebugViews/DebugView_System.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

struct ImDrawList;

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EntityWorldManager;
    class EntityWorldDebugger;
    namespace Render { class Viewport; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EngineDevUI final : public ImGuiX::IDevUI
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
        virtual void FrameEndUpdate( UpdateContext const& context ) override final;

    private:

        void DrawPopups( UpdateContext const& context );
        void DrawMenu( UpdateContext const& context );
        void DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport );
        void DrawWindows( UpdateContext const& context );
        void DrawStatusBar( UpdateContext const& context );

        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) override {}
        virtual void EndHotReload() {}

    protected:

        EntityWorldManager*                                 m_pWorldManager = nullptr;
        EntityWorldDebugger*                                m_pWorldDebugger = nullptr;

        TVector<ModalPopupMessage>                          m_modalPopups;
        Seconds                                             m_avgTimeDelta = 0.0f;
        bool                                                m_debugOverlayEnabled = false;

        SystemLogView                                       m_systemLogView;
        bool                                                m_isLogWindowOpen = false;

        DebugSettingsView                                   m_debugSettingsView;
        bool                                                m_isDebugSettingsWindowOpen = false;
    };
}
#endif