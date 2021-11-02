#include "ResourceEditor_Map.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"

//-------------------------------------------------------------------------

namespace KRG
{
    MapEditor::MapEditor( ResourceEditorContext const& context )
        : ResourceEditorWorkspace( context )
    {
        m_windowName = "Map Editor";
    }

    void MapEditor::Activate( EntityWorld* pPreviewWorld )
    {
        m_pPreviewWorld = pPreviewWorld;
    }

    void MapEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        m_pPreviewWorld = nullptr;
    }

    //-------------------------------------------------------------------------

    void MapEditor::OnNewMap()
    {

    }

    void MapEditor::OnLoadMap()
    {
        // Pick a new map
        //-------------------------------------------------------------------------

        auto const selectedMap = pfd::open_file( "Choose Map", m_editorContext.m_sourceResourceDirectory.c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
        if ( selectedMap.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( selectedMap[0].c_str() );
        ResourceID const mapToLoad = ResourcePath::FromFileSystemPath( m_editorContext.m_sourceResourceDirectory, mapFilePath );

        if ( mapToLoad != m_loadedMap )
        {
            // Should we save the current map before unloading?
            if ( IsDirty() )
            {
                // Save?
            }

            // Unload current map
            if ( m_loadedMap.IsValid() && m_pPreviewWorld->IsMapLoaded( m_loadedMap ) )
            {
                m_pPreviewWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_pPreviewWorld->LoadMap( mapToLoad );
            m_loadedMap = mapToLoad;
        }
    }

    void MapEditor::OnSaveMap()
    {

    }

    void MapEditor::OnSaveMapAs()
    {

    }

    bool MapEditor::OnSave()
    {
        return false;
    }

    //-------------------------------------------------------------------------

    void MapEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowName(), topDockID );
    }

    void MapEditor::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        /*ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( s_infoWindowName ) )
        {
            DrawWindowContents();
        }
        ImGui::End();*/
    }
}