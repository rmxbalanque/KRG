#include "SystemDebugViewController.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
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
                auto const logEntries = Log::GetLogEntries();
                for ( auto const& entry : logEntries )
                {
                    ImVec4 textColor( 1.0f, 1.0f, 1.0f, 1.0f );
                    switch ( entry.m_severity )
                    {
                        case Log::Severity::Warning:
                        textColor = Colors::Yellow.ToFloat4();
                        break;

                        case Log::Severity::Error:
                        textColor = Colors::Red.ToFloat4();
                        break;
                    }

                    ImGui::TextColored( textColor, "[%s] %s: %s", entry.m_timestamp.c_str(), entry.m_channel.c_str(), entry.m_message.c_str() );
                }

                ImGui::End();
            }
        }
    }
}
#endif