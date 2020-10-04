#include "SystemDebugViewController.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Core/Logging/Log.h"

#if _WIN32
#include <shellapi.h>
#endif

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        SystemDebugViewController::SystemDebugViewController()
        {
            auto DrawProfilingMenuOptions = [this] ( UpdateContext const& context )
            {
                #if _WIN32
                if ( ImGui::Button( "Open Profiler" ) )
                {
                    ShellExecute( 0, 0, "http://Localhost:1338", 0, 0, SW_SHOW );
                }

                if ( ImGui::Button( "Capture 30 Frames" ) )
                {
                    ShellExecute( 0, 0, "http://Localhost:1338/30", 0, 0, SW_SHOW );
                }
                #endif
            };

            m_menuCallbacks.emplace_back( DebugMenuCallback( "Microprofile", "Profiling", DrawProfilingMenuOptions ) );

            //-------------------------------------------------------------------------

            auto drawSystemLogMenuOptions = [this] ( UpdateContext const& context )
            {
                if ( ImGui::Button( "Show System Log" ) )
                {
                    m_isSystemLogWindowOpen = true;
                }
            };

            m_menuCallbacks.emplace_back( DebugMenuCallback( "Log", "System", drawSystemLogMenuOptions ) );
        }

        void SystemDebugViewController::DrawWindows( UpdateContext const& context )
        {
            // System Log
            //-------------------------------------------------------------------------

            if ( m_isSystemLogWindowOpen )
            {
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
}
#endif