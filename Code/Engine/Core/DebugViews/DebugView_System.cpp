#include "DebugView_System.h"
#include "System/Imgui/ImguiX.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace Debug
    {
        SystemDebugViewController::SystemDebugViewController()
        {
            m_menuCallbacks.emplace_back( DebugMenuCallback( "System", "System", [this] ( UpdateContext const& context ) { DrawSystemMenu( context ); } ) );
        }

        void SystemDebugViewController::DrawWindows( UpdateContext const& context )
        {
            if ( m_isSystemLogWindowOpen )
            {
                DrawSystemLogWindow( context );
            }
        }

        //-------------------------------------------------------------------------

        void SystemDebugViewController::DrawSystemMenu( UpdateContext const& context )
        {
            if ( ImGui::Button( "Show System Log" ) )
            {
                m_isSystemLogWindowOpen = true;
            }

            ImGui::Separator();

            if ( ImGui::Button( "Open Profiler" ) )
            {
                Profiling::OpenProfiler();
            }
        }

        void SystemDebugViewController::DrawSystemLogWindow( UpdateContext const& context )
        {
            KRG_ASSERT( m_isSystemLogWindowOpen );

            ImGui::SetNextWindowBgAlpha( 0.75f );
            if ( ImGui::Begin( "System Log", &m_isSystemLogWindowOpen ) )
            {
                if ( ImGui::BeginTable( "System Log Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
                {
                    ImGui::TableSetupColumn( "Time", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 55 );
                    ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 55 );
                    ImGui::TableSetupColumn( "Channel", ImGuiTableColumnFlags_WidthFixed, 60 );
                    ImGui::TableSetupColumn( "Message", ImGuiTableColumnFlags_WidthStretch );

                    //-------------------------------------------------------------------------

                    ImGui::TableHeadersRow();

                    //-------------------------------------------------------------------------

                    auto const& logEntries = Log::GetLogEntries();
                    for ( auto const& entry : logEntries )
                    {
                        ImGui::TableNextRow();

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Text( entry.m_timestamp.c_str() );

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 1 );
                        switch ( entry.m_severity )
                        {
                            case Log::Severity::Warning:
                            ImGui::TextColored( Colors::Yellow.ToFloat4(), "Warning" );
                            break;

                            case Log::Severity::Error:
                            ImGui::TextColored( Colors::Red.ToFloat4(), "Error" );
                            break;

                            case Log::Severity::Message:
                            ImGui::Text( "Message" );
                            break;
                        }

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 2 );
                        ImGui::Text( entry.m_channel.c_str() );

                        //-------------------------------------------------------------------------

                        ImGui::TableSetColumnIndex( 3 );
                        ImGui::Text( entry.m_message.c_str() );
                    }

                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }
    }
}
#endif