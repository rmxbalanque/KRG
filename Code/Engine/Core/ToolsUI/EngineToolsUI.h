#pragma once

#include "IToolsUI.h"
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

    class KRG_ENGINE_CORE_API EngineToolsUI final : public ImGuiX::IToolsUI
    {
        friend class GamePreviewer;

    public:

        void LockToWindow( String const& windowName ) { m_windowName = windowName; }

        virtual void Initialize( UpdateContext const& context ) override final;
        virtual void Shutdown( UpdateContext const& context ) override final;
        virtual void EndFrame( UpdateContext const& context ) override final;

    private:

        void DrawMenu( UpdateContext const& context, EntityWorld* pGameWorld );
        void DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport );
        void DrawWindows( UpdateContext const& context, EntityWorld* pGameWorld, ImGuiWindowClass* pWindowClass = nullptr );

        virtual void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload, TVector<ResourceID> const& resourcesToBeReloaded ) override {}
        virtual void EndHotReload() {}

        void HandleUserInput( UpdateContext const& context, EntityWorld* pGameWorld );

        void ToggleWorldPause( EntityWorld* pGameWorld );
        void SetWorldTimeScale( EntityWorld* pGameWorld, float newTimeScale );
        void ResetWorldTimeScale( EntityWorld* pGameWorld );
        void RequestWorldTimeStep( EntityWorld* pGameWorld );

    protected:

        EntityWorldManager*                                 m_pWorldManager = nullptr;
        EntityWorldDebugger*                                m_pWorldDebugger = nullptr;
        String                                              m_windowName;

        Seconds                                             m_averageDeltaTime = 0.0f;
        float                                               m_timeScale = 1.0f;
        bool                                                m_debugOverlayEnabled = false;

        SystemLogView                                       m_systemLogView;
        bool                                                m_isLogWindowOpen = false;
        bool                                                m_isDebugSettingsWindowOpen = false;
        bool                                                m_isTimeControlWindowOpen = false;
    };
}
#endif