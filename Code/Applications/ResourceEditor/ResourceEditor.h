#pragma once

#include "ResourceEditor_Model.h"
#include "ResourceEditor_DataBrowser.h"
#include "Engine/Core/DevelopmentUI.h"
#include "Tools/Core/Resource/ResourceDatabase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditor final : public ImGuiX::DevelopmentUI
    {
        struct ModalPopupMessage
        {
            String                                      m_ID;
            String                                      m_channel;
            String                                      m_message;
            Log::Severity                               m_severity;
            bool                                        m_isOpen = true;
        };

        struct ViewportSettings
        {
            bool                                        m_allowDockingOverViewport = false;
            bool                                        m_hideViewportTabBar = true;
        };

    public:

        ~ResourceEditor();

        void Initialize( UpdateContext const& context ) override;
        void Shutdown( UpdateContext const& context ) override;
        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportManager ) override final;

    private:

        // Frame updates 
        void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager );
        void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager );

        // Menus and Toolbars
        void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager );

        // Viewport
        void DrawViewportWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, char const* const pEditorViewportName = "Editor Viewport" );
        bool HasViewportToolbar() const;
        void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager );

        // Workspaces
        bool DrawWorkspaceWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ResourceEditorWorkspace* pWorkspace );
        void DrawPopups( UpdateContext const& context );

    private:

        ResourceEditorModel                 m_model;
        ViewportSettings                    m_viewportSettings;
        bool                                m_mouseWithinEditorViewport = false;
        TVector<ModalPopupMessage>          m_modalPopups;

        Resource::ResourceDatabase          m_db;
        DataBrowser*                        m_pDataBrowser = nullptr;
        float                               m_dataBrowserViewWidth = 150;
    };
}