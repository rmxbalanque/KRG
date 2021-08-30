#include "EditorTabWell.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"
#include "Workspaces/ResourceWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    void EditorTabWell::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        //// Prevent anything docking into this window, and remove the tab-bar
        //ImGuiWindowClass windowClass;
        //windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoTabBar;
        //ImGui::SetNextWindowClass( &windowClass );

        //// Create tab well window
        //ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Theme::s_backgroundColorDark );
        //ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        //ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8, 0 ) );
        //int32 const windowFlags = 0;
        //if ( ImGui::Begin( "Open Files Window", nullptr, windowFlags ) )
        //{
        //    auto const& openFiles = m_model.GetOpenFiles();
        //    if ( openFiles.empty() )
        //    {
        //        ImGui::Text( "No open files" );
        //    }
        //    else // Draw tab bar and timeline hosts
        //    {
        //        TInlineVector<ResourceEditorWorkspace*, 4> filesToClose;
        //        int32 const tabBarFlags = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
        //        if ( ImGui::BeginTabBar( "OpenFilesTabWell", tabBarFlags ) )
        //        {
        //            for ( auto& pOpenFile : openFiles )
        //            {
        //                ImGui::PushID( pOpenFile );

        //                // Draw Tab
        //                //-------------------------------------------------------------------------

        //                bool isOpened = true;
        //                int32 tabFlags = pOpenFile->IsDirty() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None;
        //                if ( ImGui::BeginTabItem( pOpenFile->GetDisplayName(), &isOpened, tabFlags ) )
        //                {
        //                    if ( m_model.IsActiveFile( pOpenFile ) )
        //                    {
        //                        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Theme::s_backgroundColorMedium );
        //                        ImGui::BeginChild( "TabContents");
        //                        {
        //                            pOpenFile->DrawTools( context, viewportManager );
        //                        }
        //                        ImGui::EndChild();
        //                        ImGui::PopStyleColor();
        //                    }
        //                    else
        //                    {
        //                        m_model.SetActiveFile( pOpenFile );
        //                    }
        //                    ImGui::EndTabItem();
        //                }

        //                // Handle close command
        //                //-------------------------------------------------------------------------

        //                if ( !isOpened )
        //                {
        //                    filesToClose.emplace_back( pOpenFile );
        //                }

        //                ImGui::PopID();
        //            }
        //            ImGui::EndTabBar();
        //        }

        //        for ( auto const& pFile : filesToClose )
        //        {
        //            m_model.CloseFile( pFile );
        //        }
        //    }
        //}
        //ImGui::End();
        //ImGui::PopStyleVar( 2 );
        //ImGui::PopStyleColor();
    }
}