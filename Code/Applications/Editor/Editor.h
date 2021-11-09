#pragma once

#include "Editor_Model.h"
#include "ResourceBrowser/ResourceBrowser.h"
#include "Engine/Core/IDevToolsUI.h"
#include "Engine/Core/DebugViews/DebugView_System.h"
#include "Tools/Core/Resource/ResourceDatabase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Editor final : public ImGuiX::IDevToolsUI
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

        ~Editor();

        void Initialize( UpdateContext const& context ) override;
        void Shutdown( UpdateContext const& context ) override;
        virtual void FrameStartUpdate( UpdateContext const& context ) override final;

    private:

        void DrawMainMenu( UpdateContext const& context );
        bool DrawWorkspace( UpdateContext const& context, EditorWorkspace* pWorkspace );
        void DrawPopups( UpdateContext const& context );

        // Hot Reload
        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) override;
        virtual void EndHotReload() override;

    private:

        EditorModel                         m_model;
        bool                                m_mouseWithinEditorViewport = false;
        TVector<ModalPopupMessage>          m_modalPopups;

        Resource::ResourceDatabase          m_db;
        ResourceBrowser*                    m_pResourceBrowser = nullptr;
        float                               m_dataBrowserViewWidth = 150;

        SystemLogView                       m_systemLogView;
        bool                                m_isSystemLogWindowOpen = false;

        DebugSettingsView                   m_debugSettingsView;
        bool                                m_isDebugSettingsWindowOpen = false;

        bool                                m_isResourceBrowserWindowOpen = true;
        bool                                m_isResourceLogWindowOpen = false;
        bool                                m_isResourceReferenceTrackerWindowOpen = false;
    };
}