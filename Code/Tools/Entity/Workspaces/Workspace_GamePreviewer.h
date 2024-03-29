#pragma once

#include "Tools/Entity/_Module/API.h"
#include "Tools/Core/Workspaces/EditorWorkspace.h"
#include "Engine/Core/ToolsUI/EngineToolsUI.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_ENTITY_API GamePreviewer final : public EditorWorkspace
    {
    public:

        GamePreviewer( WorkspaceInitializationContext const& context, EntityWorld* pWorld );

        void LoadMapToPreview( ResourceID mapResourceID );

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        virtual uint32 GetID() const override { return 0xFFFFFFFE; }
        virtual bool HasWorkspaceToolbar() const override;
        virtual bool HasViewportToolbar() const override { return false; }
        virtual bool HasViewportOrientationGuide() const override { return false; }
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;

        virtual void DrawWorkspaceToolbar( UpdateContext const& context ) override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) override;

    private:

        ResourceID                      m_loadedMap;
        EngineToolsUI                   m_engineToolsUI;
    };
}