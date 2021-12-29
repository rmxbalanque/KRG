#pragma once

#include "EditorContext.h"
#include "ResourceBrowser/ResourceBrowser.h"
#include "Engine/Core/ToolsUI/IToolsUI.h"
#include "Engine/Core/DebugViews/DebugView_System.h"
#include "Tools/Core/Resource/ResourceDatabase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EditorUI final : public ImGuiX::IToolsUI
    {
        struct ModalPopupMessage
        {
            String                                      m_ID;
            String                                      m_channel;
            String                                      m_message;
            Log::Severity                               m_severity;
            bool                                        m_isOpen = true;
        };

    public:

        ~EditorUI();

        void Initialize( UpdateContext const& context ) override;
        void Shutdown( UpdateContext const& context ) override;

    private:

        void DrawMainMenu( UpdateContext const& context );
        void DrawPopups( UpdateContext const& context );
        bool DrawWorkspaceWindow( UpdateContext const& context, EditorWorkspace* pWorkspace );

        virtual void StartFrame( UpdateContext const& context ) override final;
        virtual void EndFrame( UpdateContext const& context ) override final;
        virtual void Update( UpdateContext const& context ) override final;

        // Hot Reload
        virtual void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded ) override;
        virtual void EndHotReload() override;

    private:

        EditorContext                       m_context;
        ImGuiWindowClass                    m_editorWindowClass;
        TVector<ModalPopupMessage>          m_modalPopups;

        ResourceBrowser*                    m_pResourceBrowser = nullptr;
        EventBindingID                      m_resourceDatabaseUpdateEventBindingID;
        float                               m_dataBrowserViewWidth = 150;

        SystemLogView                       m_systemLogView;
        bool                                m_isSystemLogWindowOpen = false;
        bool                                m_isDebugSettingsWindowOpen = false;

        bool                                m_isResourceBrowserWindowOpen = true;
        bool                                m_isResourceLogWindowOpen = false;
        bool                                m_isResourceReferenceTrackerWindowOpen = false;
        bool                                m_isPhysicsMaterialDatabaseWindowOpen = false;
        bool                                m_isImguiDemoWindowOpen = false;
    };
}