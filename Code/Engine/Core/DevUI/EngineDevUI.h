#pragma once

#include "IDevUI.h"
#include "Engine/Core/DebugViews/DebugView_System.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

struct ImDrawList;
struct ImGuiWindowClass;

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EntityWorld;
    class EntityWorldManager;
    class EntityWorldDebugger;
    namespace Render { class Viewport; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EngineDevUI final : public ImGuiX::IDevUI
    {
        friend class GamePreviewer;

        struct ModalPopupMessage
        {
            String                                          m_ID;
            String                                          m_channel;
            String                                          m_message;
            Log::Severity                                   m_severity;
            bool                                            m_isOpen = true;
        };

    public:

        void LockToWindow( String const& windowName ) { m_windowName = windowName; }

        virtual void Initialize( UpdateContext const& context ) override final;
        virtual void Shutdown( UpdateContext const& context ) override final;
        virtual void EndFrame( UpdateContext const& context ) override final;

    private:

        void DrawPopups( UpdateContext const& context );
        void DrawMenu( UpdateContext const& context );
        void DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport );
        void DrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass = nullptr );
        void DrawStatusBar( UpdateContext const& context, EntityWorld* pGameWorld );

        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) override {}
        virtual void EndHotReload() {}

    protected:

        EntityWorldManager*                                 m_pWorldManager = nullptr;
        EntityWorldDebugger*                                m_pWorldDebugger = nullptr;
        String                                              m_windowName;

        TVector<ModalPopupMessage>                          m_modalPopups;
        Seconds                                             m_averageDeltaTime = 0.0f;
        float                                               m_timeScale = 1.0f;
        bool                                                m_stepRequested = false;
        bool                                                m_debugOverlayEnabled = false;

        SystemLogView                                       m_systemLogView;
        bool                                                m_isLogWindowOpen = false;

        DebugSettingsView                                   m_debugSettingsView;
        bool                                                m_isDebugSettingsWindowOpen = false;
    };
}
#endif