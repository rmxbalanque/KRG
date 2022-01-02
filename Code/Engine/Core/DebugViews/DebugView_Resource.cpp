#include "DebugView_Resource.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Systems/SystemRegistry.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    ResourceDebugView::ResourceDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Resource", [this] ( EntityWorldUpdateContext const& context ) { DrawResourceMenu( context ); } ) );
    }

    void ResourceDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        EntityWorldDebugView::Initialize( systemRegistry, pWorld );
        m_pResourceSystem = systemRegistry.GetSystem<ResourceSystem>();
    }

    void ResourceDebugView::Shutdown()
    {
        m_pResourceSystem = nullptr;
        EntityWorldDebugView::Shutdown();
    }

    void ResourceDebugView::DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( m_isHistoryWindowOpen )
        {
            ImGui::SetNextWindowBgAlpha( 0.75f );
            DrawResourceLogWindow( m_pResourceSystem, &m_isHistoryWindowOpen );
        }

        if ( m_isReferenceTrackerWindowOpen )
        {
            ImGui::SetNextWindowBgAlpha( 0.75f );
            DrawReferenceTrackerWindow( m_pResourceSystem, &m_isReferenceTrackerWindowOpen );
        }
    }

    void ResourceDebugView::DrawResourceMenu( EntityWorldUpdateContext const& context )
    {
        if ( ImGui::MenuItem( "Show Request History" ) )
        {
            m_isHistoryWindowOpen = true;
        }

        if ( ImGui::MenuItem( "Show Resource Tracker" ) )
        {
            m_isReferenceTrackerWindowOpen = true;
        }
    }

    //-------------------------------------------------------------------------

    void ResourceDebugView::DrawReferenceTrackerWindow( ResourceSystem* pResourceSystem, bool* pIsOpen )
    {
        KRG_ASSERT( pResourceSystem != nullptr );

        if ( ImGui::Begin( "Resource Reference Tracker", pIsOpen ) )
        {
            if ( ImGui::BeginTable( "Resource Reference Tracker Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30 );
                ImGui::TableSetupColumn( "Refs", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 24 );
                ImGui::TableSetupColumn( "Status", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );
                ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto const& recordTuple : pResourceSystem->m_resourceRecords )
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

                    switch ( pRecord->m_loadingStatus )
                    {
                        case LoadingStatus::Unloaded:
                        {
                            ImGui::Text( "Unloaded" );
                        }
                        break;

                        case LoadingStatus::Loading:
                        {
                            ImGui::TextColored( Colors::Yellow.ToFloat4(), "Loaded" );
                        }
                        break;

                        case LoadingStatus::Loaded:
                        {
                            ImGui::TextColored( Colors::LimeGreen.ToFloat4(), "Loaded" );
                        }
                        break;

                        case LoadingStatus::Unloading:
                        {
                            ImGui::Text( "Unloading" );
                        }
                        break;

                        case LoadingStatus::Failed :
                        {
                            ImGui::TextColored( Colors::Red.ToFloat4(), "Loaded" );
                        }
                        break;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 3 );
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
                                ImGui::TextColored( Colors::Coral.ToFloat4(), "Install Dependency: %u", requesterID.GetInstallDependencyResourcePathID() );
                            }
                            else // Normal request
                            {
                                ImGui::TextColored( Colors::Lime.ToFloat4(), "Entity: %u", requesterID.GetID() );
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

    void ResourceDebugView::DrawResourceLogWindow( ResourceSystem* pResourceSystem, bool* pIsOpen )
    {
        if ( ImGui::Begin( "Resource Request History", pIsOpen ) )
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

                for ( auto i = (int32) pResourceSystem->m_history.size() - 1; i >= 0; i-- )
                {
                    auto const& entry = pResourceSystem->m_history[i];

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
}
#endif