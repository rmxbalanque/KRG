#include "ResourceServerUI.h"
#include "ResourceServer.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "System/DevTools/ImguiX.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceServerUI::ResourceServerUI( ResourceServer* pServer )
        : m_pResourceServer( pServer )
    {
        KRG_ASSERT( m_pResourceServer != nullptr );
    }

    void ResourceServerUI::Draw()
    {
        // Create dock space
        //-------------------------------------------------------------------------

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::Begin( "DockSpace", nullptr, windowFlags );
        {
            ImGuiDockNodeFlags const dockSpaceFlags = ImGuiDockNodeFlags_None;
            ImGuiID const dockspaceID = ImGui::GetID( "ResourceServerDockSpace" );
            ImGui::DockSpace( dockspaceID, ImVec2( 0.0f, 0.0f ), dockSpaceFlags );
        }
        ImGui::PopStyleVar( 3 );
        ImGui::End();

        // Draw windows
        //-------------------------------------------------------------------------

        DrawServerInfo();
        DrawPendingRequests();
        DrawCompletedRequests();
        DrawWorkerStatus();
    }

    void ResourceServerUI::DrawPendingRequests()
    {
        if ( ImGui::Begin( "Pending Requests" ) )
        {
            if ( ImGui::BeginTable( "Pending Request Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
            {
                ImGui::TableSetupColumn( "ClientID", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 50 );
                ImGui::TableSetupColumn( "Resource ID", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                for ( auto const& pPendingRequest : m_pResourceServer->GetPendingRequests() )
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    if ( pPendingRequest->IsInternalRequest() )
                    {
                        ImGui::Text( "File System Watcher" );
                    }
                    else
                    {
                        ImGui::Text( "%llu", pPendingRequest->GetClientID() );
                    }

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( pPendingRequest->GetResourceID().c_str() );
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void ResourceServerUI::DrawCompletedRequests()
    {
        if ( ImGui::Begin( "Completed Requests" ) )
        {
            float const OutputFieldHeight = 75;
            float const TableHeight = ImGui::GetContentRegionAvail().y - OutputFieldHeight - 32;
            float const buttonWidth = 120;
            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
            float const textfieldWidth = ImGui::GetWindowContentRegionWidth() - ( ( buttonWidth + itemSpacing ) * 4 );

            //-------------------------------------------------------------------------
            // Table
            //-------------------------------------------------------------------------

            ImGuiX::ScopedFont const BigScopedFont( ImGuiX::Font::Small );

            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Theme::s_accentColorDark );
            if ( ImGui::BeginTable( "Completed Requests Table", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX, ImVec2( 0, TableHeight ) ) )
            {
                ImGui::TableSetupColumn( "##Status", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 10 );
                ImGui::TableSetupColumn( "Client", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 120 );
                ImGui::TableSetupColumn( "Source", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Destination", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Up To Date", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );
                ImGui::TableSetupColumn( "Compile", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                for ( auto const& pCompletedRequest : m_pResourceServer->GetCompletedRequests() )
                {
                    ImVec4 itemColor;
                    ImGui::TableNextRow();

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 0 );
                    switch ( pCompletedRequest->GetStatus() )
                    {
                        case CompilationRequest::Status::Succeeded:
                        {
                            itemColor = ImVec4( 0, 1, 0, 1 );
                            ImGui::TextColored( itemColor, KRG_ICON_CHECK );
                        }
                        break;

                        case CompilationRequest::Status::SucceededWithWarnings:
                        {
                            itemColor = ImVec4( 1, 1, 0, 1 );
                            ImGui::TextColored( itemColor, KRG_ICON_EXCLAMATION_TRIANGLE );
                        }
                        break;

                        case CompilationRequest::Status::Failed:
                        {
                            itemColor = ImVec4( 1, 0, 0, 1 );
                            ImGui::TextColored( itemColor, KRG_ICON_TIMES );
                        }
                        break;

                        default:
                        break;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 1 );
                    if ( pCompletedRequest->IsInternalRequest() )
                    {
                        ImGui::Text( "File System Watcher" );
                    }
                    else
                    {
                        ImGui::Text( "%llu", pCompletedRequest->GetClientID() );
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 2 );
                    ImGui::TextColored( itemColor, pCompletedRequest->GetSourceFilePath().c_str() );

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 3 );
                    bool const item_is_selected = ( pCompletedRequest == m_pSelectedCompletedRequest );
                    if ( ImGui::Selectable( pCompletedRequest->GetDestinationFilePath().c_str(), item_is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2( 0, 0 ) ) )
                    {
                        m_pSelectedCompletedRequest = pCompletedRequest;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 4 );
                    ImGui::Text( "%.3fms", pCompletedRequest->GetUptoDateCheckElapsedTime().ToFloat() );

                    ImGui::TableSetColumnIndex( 5 );
                    ImGui::Text( "%.3fms", pCompletedRequest->GetCompilationElapsedTime().ToFloat() );
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleColor();

            ImGui::Separator();

            //-------------------------------------------------------------------------
            // Info Panel
            //-------------------------------------------------------------------------

            char emptyBuffer[2] = { 0 };
            bool const hasSelectedItem = m_pSelectedCompletedRequest != nullptr;

            {
                ImGuiX::ScopedFont const scopedFont( ImGuiX::Font::Medium );

                if ( m_pSelectedCompletedRequest != nullptr )
                {
                    ImGui::SetNextItemWidth( textfieldWidth );
                    ImGui::InputText( "##Name", const_cast<char*>( m_pSelectedCompletedRequest->GetCompilerArgs() ), strlen( m_pSelectedCompletedRequest->GetCompilerArgs() ), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly );
                }
                else
                {
                    ImGui::SetNextItemWidth( textfieldWidth );
                    ImGui::InputText( "##Name", emptyBuffer, 2, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly );
                }

                ImGui::SameLine();
                if( ImGui::Button( KRG_ICON_FILES_O " Copy Args", ImVec2( buttonWidth, 0 ) ) )
                {
                    if ( hasSelectedItem )
                    {
                        String path( "-compile " );
                        path += m_pSelectedCompletedRequest->GetCompilerArgs();
                        ImGui::SetClipboardText( path.c_str() );
                    }
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_FILE " Go To Source", ImVec2( buttonWidth, 0 ) ) )
                {
                    if ( hasSelectedItem )
                    {
                        FileSystem::OpenFileInExplorer( m_pSelectedCompletedRequest->GetSourceFilePath() );
                    }
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_FILE_ARCHIVE_O " Go To Output", ImVec2( buttonWidth, 0 ) ) )
                {
                    if ( hasSelectedItem )
                    {
                        FileSystem::OpenFileInExplorer( m_pSelectedCompletedRequest->GetDestinationFilePath() );
                    }
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_TRASH " Clear History", ImVec2( buttonWidth, 0 ) ) )
                {
                    m_pResourceServer->RequestCleanupOfCompletedRequests();
                }
            }

            //-------------------------------------------------------------------------

            if ( m_pSelectedCompletedRequest != nullptr )
            {
                ImGui::InputTextMultiline( "##Output", const_cast<char*>( m_pSelectedCompletedRequest->GetLog() ), strlen( m_pSelectedCompletedRequest->GetLog() ), ImVec2( ImGui::GetWindowContentRegionWidth(), OutputFieldHeight ), ImGuiInputTextFlags_ReadOnly );
            }
            else
            {
                ImGui::InputTextMultiline( "##Output", emptyBuffer, 255, ImVec2( ImGui::GetWindowContentRegionWidth(), OutputFieldHeight ), ImGuiInputTextFlags_ReadOnly );
            }
        }
        ImGui::End();
    }

    void ResourceServerUI::DrawWorkerStatus()
    {
        if ( ImGui::Begin( "Worker Status" ) )
        {
            int32 const numWorkers = m_pResourceServer->GetNumWorkers();
            for ( auto i = 0; i < numWorkers; i++ )
            {
                ImGui::Text( "Worker %02d: ", i );
                ImGui::SameLine();
                if ( m_pResourceServer->GetWorkerStatus( i ) == ResourceServerWorker::Status::Compiling )
                {
                    ImGui::TextColored( ImGuiX::ConvertColor( Colors::LimeGreen ), m_pResourceServer->GetCompilationTaskResourceID( i ).c_str() );
                }
                else
                {
                    ImGui::TextColored( ImGuiX::ConvertColor( Colors::Yellow ), "Idle" );
                }
            }
        }
        ImGui::End();
    }

    void ResourceServerUI::DrawServerInfo()
    {
        if ( ImGui::Begin( "Server Info" ) )
        {
            ImGui::Text( "Source Data Path: %s", m_pResourceServer->GetSourceDataDir().c_str() );
            ImGui::Text( "Compiled Data Path: %s", m_pResourceServer->GetCompiledDataDir().c_str() );
            ImGui::Text( "IP Address: %s", m_pResourceServer->GetNetworkAddress().c_str() );

            ImGui::NewLine();

            //-------------------------------------------------------------------------

            if ( ImGui::BeginTable( "Registered Compilers Table", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
            {
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 150 );
                ImGui::TableSetupColumn( "Ver", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15 );
                ImGui::TableSetupColumn( "Module", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Output Types", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 80 );
                ImGui::TableSetupColumn( "Virtual Types", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 0 );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                for ( auto const& compilerInfo : m_pResourceServer->GetRegisteredCompilers() )
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( compilerInfo.m_name.c_str() );

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( "%d", compilerInfo.m_version );

                    ImGui::TableSetColumnIndex( 2 );
                    ImGui::Text( compilerInfo.m_sourceModule.c_str() );

                    //-------------------------------------------------------------------------

                    char str[32] = { 0 };

                    ImGui::TableSetColumnIndex( 3 );
                    for ( auto const& type : compilerInfo.m_outputTypes )
                    {
                        type.GetString( str );
                        ImGui::Text( str );
                        ImGui::SameLine();
                    }

                    ImGui::TableSetColumnIndex( 4 );
                    for ( auto const& type : compilerInfo.m_virtualTypes )
                    {
                        type.GetString( str );
                        ImGui::Text( str );
                        ImGui::SameLine();
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}