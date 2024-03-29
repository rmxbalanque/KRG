#include "ResourceServerUI.h"
#include "ResourceServer.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    static char const* const g_pendingRequestsWindowName = "Pending Requests";
    static char const* const g_completedRequestsWindowName = "Completed Requests";
    static char const* const g_workerStatusWindowName = "Worker Status";
    static char const* const g_serverInfoWindowName = "Server Info";
    static char const* const g_connectionInfoWindowName = "Connected Clients";

    //-------------------------------------------------------------------------

    ResourceServerUI::ResourceServerUI( ResourceServer* pServer )
        : m_pResourceServer( pServer )
    {
        KRG_ASSERT( m_pResourceServer != nullptr );
    }

    void ResourceServerUI::Draw()
    {
        // Create dock space
        //-------------------------------------------------------------------------

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

        ImGuiWindowFlags const windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin( "DockSpace", nullptr, windowFlags );
        {
            ImGuiID const dockspaceID = ImGui::GetID( "ResourceServerDockSpace" );

            // Initial Layout
            if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            {
                ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );
                ImGui::DockBuilderSetNodeSize( dockspaceID, viewport->Size );

                ImGuiID topDockID = 0, topRightDockID = 0;
                ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.7f, nullptr, &topDockID );
                ImGuiID topLeftDockID = ImGui::DockBuilderSplitNode( topDockID, ImGuiDir_Left, 0.5f, nullptr, &topRightDockID );

                ImGui::DockBuilderDockWindow( g_workerStatusWindowName, topRightDockID );
                ImGui::DockBuilderDockWindow( g_connectionInfoWindowName, topLeftDockID );
                ImGui::DockBuilderDockWindow( g_serverInfoWindowName, topLeftDockID );
                ImGui::DockBuilderDockWindow( g_pendingRequestsWindowName, topLeftDockID );
                ImGui::DockBuilderDockWindow( g_completedRequestsWindowName, bottomDockID );

                ImGui::DockBuilderFinish( dockspaceID );
            }

            ImGui::DockSpace( dockspaceID, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_None );
        }
        ImGui::PopStyleVar( 3 );
        ImGui::End();

        // Draw windows
        //-------------------------------------------------------------------------

        DrawServerInfo();
        DrawConnectionInfo();
        DrawCompletedRequests();
        DrawWorkerStatus();
    }

    void ResourceServerUI::DrawCompletedRequests()
    {
        if ( ImGui::Begin( g_completedRequestsWindowName ) )
        {
            constexpr static float const compilationLogFieldHeight = 125;
            constexpr static float const buttonWidth = 140;
            float const tableHeight = ImGui::GetContentRegionAvail().y - compilationLogFieldHeight - ImGui::GetFrameHeight() - 16;
            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
            float const textfieldWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x - ( ( buttonWidth + itemSpacing ) * 5 );

            //-------------------------------------------------------------------------
            // Table
            //-------------------------------------------------------------------------

            ImGuiX::ScopedFont const BigScopedFont( ImGuiX::Font::Small );

            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
            if ( ImGui::BeginTable( "Completed Requests Table", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY, ImVec2( 0, tableHeight ) ) )
            {
                auto const& activeRequests = m_pResourceServer->GetActiveRequests();
                auto const& pendingRequests = m_pResourceServer->GetPendingRequests();
                auto const& completedRequests = m_pResourceServer->GetCompletedRequests();

                m_combinedRequests.clear();
                m_combinedRequests.reserve( activeRequests.size() + pendingRequests.size() + completedRequests.size() );
                
                m_combinedRequests.insert( m_combinedRequests.end(), activeRequests.begin(), activeRequests.end() );
                m_combinedRequests.insert( m_combinedRequests.end(), pendingRequests.begin(), pendingRequests.end() );
                m_combinedRequests.insert( m_combinedRequests.end(), completedRequests.begin(), completedRequests.end() );


                auto SortPredicate = [] ( CompilationRequest const* pRequestA, CompilationRequest const* pRequestB ) { return pRequestA->GetTimeRequested() < pRequestB->GetTimeRequested(); };
                eastl::sort( m_combinedRequests.begin(), m_combinedRequests.end(), SortPredicate );

                //-------------------------------------------------------------------------

                ImGui::TableSetupColumn( "##Status", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 18 );
                ImGui::TableSetupColumn( "Client", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 110 );
                ImGui::TableSetupColumn( "Source", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Destination", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30 );
                ImGui::TableSetupColumn( "Up To Date", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );
                ImGui::TableSetupColumn( "Compile", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );
                ImGui::TableSetupScrollFreeze( 0, 1 );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                clipper.Begin( (int32) m_combinedRequests.size() );
                while ( clipper.Step() )
                {
                    for ( int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++ )
                    {
                        CompilationRequest const* pRequest = m_combinedRequests[i];
                        ImGui::PushID( pRequest );

                        ImVec4 itemColor;
                        ImGui::TableNextRow();

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 0 );
                        switch ( pRequest->GetStatus() )
                        {
                            case CompilationRequest::Status::Pending:
                            {
                                itemColor = Colors::LightGray.ToFloat4();
                                ImGui::TextColored( itemColor, KRG_ICON_CLOCK );
                            }
                            break;

                            case CompilationRequest::Status::Compiling:
                            {
                                itemColor = Colors::Cyan.ToFloat4();
                                ImGui::TextColored( itemColor, KRG_ICON_COG );
                            }
                            break;

                            case CompilationRequest::Status::Succeeded:
                            {
                                itemColor = Colors::Lime.ToFloat4();
                                ImGui::TextColored( itemColor, KRG_ICON_CHECK );
                            }
                            break;

                            case CompilationRequest::Status::SucceededWithWarnings:
                            {
                                itemColor = Colors::Yellow.ToFloat4();
                                ImGui::TextColored( itemColor, KRG_ICON_EXCLAMATION_TRIANGLE );
                            }
                            break;

                            case CompilationRequest::Status::Failed:
                            {
                                itemColor = Colors::Red.ToFloat4();
                                ImGui::TextColored( itemColor, KRG_ICON_TIMES );
                            }
                            break;

                            default:
                            break;
                        }

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 1 );
                        if ( pRequest->IsInternalRequest() )
                        {
                            ImGui::Text( "File System Watcher" );
                        }
                        else
                        {
                            ImGui::Text( "%llu", pRequest->GetClientID() );
                        }

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 2 );
                        ImGui::TextColored( itemColor, pRequest->GetSourceFilePath().c_str() );

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 3 );
                        bool const isItemSelected = ( pRequest == m_pSelectedCompletedRequest );
                        if ( ImGui::Selectable( pRequest->GetDestinationFilePath().c_str(), isItemSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2( 0, 0 ) ) )
                        {
                            if ( pRequest->IsComplete() )
                            {
                                m_pSelectedCompletedRequest = pRequest;
                            }
                        }

                        //-------------------------------------------------------------------------

                        auto const resourceTypeStr = pRequest->GetResourceID().GetResourceTypeID().ToString();
                        ImGui::TableSetColumnIndex( 4 );
                        ImGui::Text( "%s", resourceTypeStr.c_str() );

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 5 );
                        ImGui::Text( "%.3fms", pRequest->GetUptoDateCheckElapsedTime().ToFloat() );

                        ImGui::TableSetColumnIndex( 6 );
                        ImGui::Text( "%.3fms", pRequest->GetCompilationElapsedTime().ToFloat() );

                        ImGui::PopID();
                    }
                }

                // Auto scroll the table
                if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
                {
                    ImGui::SetScrollHereY( 1.0f );
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

                ImGui::BeginDisabled( !hasSelectedItem );
                ImGui::SameLine();
                if( ImGui::Button( KRG_ICON_COPY "Copy Args", ImVec2( buttonWidth, 0 ) ) )
                {
                    String path( "-compile " );
                    path += m_pSelectedCompletedRequest->GetCompilerArgs();
                    ImGui::SetClipboardText( path.c_str() );
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_COG "Recompile", ImVec2( buttonWidth, 0 ) ) )
                {
                    m_pResourceServer->RecompileResource( m_pSelectedCompletedRequest->GetResourceID() );
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_FILE "Source File", ImVec2( buttonWidth, 0 ) ) )
                {
                    FileSystem::OpenInExplorer( m_pSelectedCompletedRequest->GetSourceFilePath() );
                }

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_FILE_ARCHIVE "Compiled File", ImVec2( buttonWidth, 0 ) ) )
                {
                    FileSystem::OpenInExplorer( m_pSelectedCompletedRequest->GetDestinationFilePath() );
                }

                ImGui::EndDisabled();

                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_TRASH "Clear History", ImVec2( buttonWidth, 0 ) ) )
                {
                    m_pResourceServer->RequestCleanupOfCompletedRequests();
                }
            }

            //-------------------------------------------------------------------------

            if ( m_pSelectedCompletedRequest != nullptr )
            {
                ImGui::InputTextMultiline( "##Output", const_cast<char*>( m_pSelectedCompletedRequest->GetLog() ), strlen( m_pSelectedCompletedRequest->GetLog() ), ImVec2( ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x, compilationLogFieldHeight ), ImGuiInputTextFlags_ReadOnly );
            }
            else
            {
                ImGui::InputTextMultiline( "##Output", emptyBuffer, 255, ImVec2( ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x, compilationLogFieldHeight ), ImGuiInputTextFlags_ReadOnly );
            }
        }
        ImGui::End();
    }

    void ResourceServerUI::DrawWorkerStatus()
    {
        if ( ImGui::Begin( g_workerStatusWindowName ) )
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
        if ( ImGui::Begin( g_serverInfoWindowName ) )
        {
            ImGui::Text( "Raw Resource Path: %s", m_pResourceServer->GetRawResourceDir().c_str() );
            ImGui::Text( "Compiled Resource Path: %s", m_pResourceServer->GetCompiledResourceDir().c_str() );
            ImGui::Text( "IP Address: %s:%d", m_pResourceServer->GetNetworkAddress().c_str(), m_pResourceServer->GetNetworkPort() );

            ImGui::NewLine();

            //-------------------------------------------------------------------------

            if ( ImGui::BeginTable( "Registered Compilers Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
            {
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 150 );
                ImGui::TableSetupColumn( "Ver", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 25 );
                ImGui::TableSetupColumn( "Output Types", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Virtual Types", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                auto const pCompilerRegistry = m_pResourceServer->GetCompilerRegistry();

                for ( auto const& pCompiler : pCompilerRegistry->GetRegisteredCompilers() )
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( pCompiler->GetName().c_str() );

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( "%d", pCompiler->GetVersion() );

                    //-------------------------------------------------------------------------

                    char str[32] = { 0 };

                    ImGui::TableSetColumnIndex( 2 );
                    for ( auto const& type : pCompiler->GetOutputTypes() )
                    {
                        type.GetString( str );
                        ImGui::Text( str );
                        ImGui::SameLine();
                    }

                    ImGui::TableSetColumnIndex( 3 );
                    for ( auto const& type : pCompiler->GetVirtualTypes() )
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

    void ResourceServerUI::DrawConnectionInfo()
    {
        if ( ImGui::Begin( g_connectionInfoWindowName ) )
        {
            if ( ImGui::BeginTable( "ConnectionInfo", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
            {
                ImGui::TableSetupColumn( "Client ID", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 150 );
                ImGui::TableSetupColumn( "IP", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                int32 const numConnectedClients = m_pResourceServer->GetNumConnectedClients();
                for ( int32 i = 0; i < numConnectedClients; i++ )
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( "%u", m_pResourceServer->GetClientID( i ) );

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( m_pResourceServer->GetConnectedClientAddress( i ).c_str() );
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}