#include "TabWell.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"
#include "Applications/Editor/Editor/EditorFileTab.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void TabWell::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Prevent anything docking into this window, and remove the tab-bar
        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass( &windowClass );

        // Create tab well window
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        int32 const windowFlags = 0;
        if ( ImGui::Begin( "Open Files Window", nullptr, windowFlags ) )
        {
            auto const& openFiles = m_model.GetOpenFiles();
            if ( openFiles.empty() )
            {
                ImGui::Text( "No open files" );
            }
            else // Draw tab bar and timeline hosts
            {
                TInlineVector<EditorFileTab*, 4> filesToClose;
                int32 const tabBarFlags = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
                if ( ImGui::BeginTabBar( "OpenFilesTabWell", tabBarFlags ) )
                {
                    for ( auto& pOpenFile : openFiles )
                    {
                        bool isOpened = true;
                        int32 tabFlags = pOpenFile->IsDirty() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None;
                        if ( ImGui::BeginTabItem( pOpenFile->GetDisplayName(), &isOpened, tabFlags ) )
                        {
                            m_model.SetActiveFile( pOpenFile );
                            pOpenFile->DrawTools( context, viewportManager );
                            ImGui::EndTabItem();
                        }

                        if ( !isOpened )
                        {
                            filesToClose.emplace_back( pOpenFile );
                        }
                    }
                    ImGui::EndTabBar();
                }

                for ( auto const& pFile : filesToClose )
                {
                    m_model.CloseFile( pFile );
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}