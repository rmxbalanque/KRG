#include "Workspace_GamePreviewer.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG
{
    GamePreviewer::GamePreviewer( EditorContext const& context, EntityWorld* pWorld )
        : EditorWorkspace( context, pWorld )
    {}

    void GamePreviewer::LoadMapToPreview( ResourceID mapResourceID )
    {
        if ( mapResourceID != m_loadedMap )
        {
            // Unload current map
            if ( m_loadedMap.IsValid() && m_pWorld->IsMapLoaded( m_loadedMap ) )
            {
                m_pWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_loadedMap = mapResourceID;
            m_pWorld->LoadMap( m_loadedMap );
            SetDisplayName( m_loadedMap.GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory ).GetFileNameWithoutExtension() );
        }
    }

    void GamePreviewer::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );
        m_engineDevUI.Initialize( context );
        m_engineDevUI.LockToWindow( GetViewportWindowID() );
    }

    void GamePreviewer::Shutdown( UpdateContext const& context )
    {
        KRG_ASSERT( m_loadedMap.IsValid() );
        m_pWorld->UnloadMap( m_loadedMap );

        m_engineDevUI.Shutdown( context );
        EditorWorkspace::Shutdown( context );
    }

    //-------------------------------------------------------------------------

    void GamePreviewer::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );
    }

    void GamePreviewer::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineDevUI.DrawWindows( context, pWindowClass );
    }

    void GamePreviewer::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineDevUI.DrawOverlayElements( context, pViewport );
    }

    void GamePreviewer::DrawWorkspaceToolbar( UpdateContext const& context )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineDevUI.DrawMenu( context );
    }
}