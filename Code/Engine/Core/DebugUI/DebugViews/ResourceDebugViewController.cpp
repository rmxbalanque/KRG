#include "ResourceDebugViewController.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Imgui/ImguiSystem.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    ResourceDebugViewController::ResourceDebugViewController()
    {
        m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "Resource", "Resource", [this] ( UpdateContext const& context ) { DrawResourceMenu( context ); } ) );
    }

    void ResourceDebugViewController::DrawWindows( UpdateContext const& context )
    {
        if ( m_isHistoryWindowOpen )
        {
            DrawHistoryWindow( context );
        }

        if ( m_isReferenceTrackerWindowOpen )
        {
            DrawReferenceTrackerWindow( context );
        }
    }

    //-------------------------------------------------------------------------

    void ResourceDebugViewController::DrawResourceMenu( UpdateContext const& context )
    {
        if ( ImGui::Button( "Show Request History" ) )
        {
            m_isHistoryWindowOpen = true;
        }

        if ( ImGui::Button( "Show Resource Tracker" ) )
        {
            m_isReferenceTrackerWindowOpen = true;
        }
    }

    void ResourceDebugViewController::DrawHistoryWindow( UpdateContext const& context )
    {
        KRG_ASSERT( m_isHistoryWindowOpen );

        ImGui::SetNextWindowBgAlpha( 0.75f );
        if ( ImGui::Begin( "Resource Request History", &m_isHistoryWindowOpen ) )
        {
            if ( ImGui::BeginTable( "Resource Request History Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Time", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 50 );
                ImGui::TableSetupColumn( "Request", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 45 );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30 );
                ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto i = (int32) m_pResourceSystem->m_history.size() - 1; i >= 0; i-- )
                {
                    auto const& entry = m_pResourceSystem->m_history[i];

                    ImGui::TableNextRow();

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( entry.m_time.GetTimeDetailed().c_str() );

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 1 );
                    switch ( entry.m_type )
                    {
                        case ResourceSystem::PendingRequest::Type::Load:
                        {
                            ImGui::TextColored( Colors::LimeGreen.ToFloat4(), "Load" );
                        }
                        break;

                        case ResourceSystem::PendingRequest::Type::Unload:
                        {
                            ImGui::TextColored( Colors::OrangeRed.ToFloat4(), "Unload" );
                        }
                        break;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 2 );
                    ImGui::Text( entry.m_ID.GetResourceTypeID().ToString().c_str() );

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 3 );
                    ImGui::Text( entry.m_ID.c_str() );
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void ResourceDebugViewController::DrawReferenceTrackerWindow( UpdateContext const& context )
    {
        KRG_ASSERT( m_isReferenceTrackerWindowOpen );

        ImGui::SetNextWindowBgAlpha( 0.75f );
        if ( ImGui::Begin( "Resource Reference Tracker", &m_isReferenceTrackerWindowOpen ) )
        {
            if ( ImGui::BeginTable( "Resource Reference Tracker Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30 );
                ImGui::TableSetupColumn( "Refs", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 24);
                ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto const& recordTuple : m_pResourceSystem->m_resourceRecords )
                {
                    ResourceRecord const* pRecord = recordTuple.second;

                    ImGui::TableNextRow();

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( pRecord->GetResourceTypeID().ToString().c_str() );

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( "%d", pRecord->m_references.size() );

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 2 );
                    if ( ImGui::TreeNode( recordTuple.second->GetResourceID().c_str() ) )
                    {
                        for ( auto const& requesterID : pRecord->m_references )
                        {
                            if ( requesterID.IsManualRequest() )
                            {
                                ImGui::TextColored( Colors::Aqua.ToFloat4(), "Manual Request" );
                            }
                            else if ( requesterID.IsInstallDependencyRequest() )
                            {
                                ImGui::TextColored( Colors::Coral.ToFloat4(), "Install Dependency: %u", requesterID.GetInstallDependencyDataPathID() );
                            }
                            else // Normal request
                            {
                                ImGui::TextColored( Colors::Lime.ToFloat4(), "Entity: %s", requesterID.ToString().c_str() );
                            }
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
#endif