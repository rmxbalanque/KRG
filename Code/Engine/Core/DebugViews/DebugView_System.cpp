#include "DebugView_System.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    SystemDebugView::SystemDebugView()
    {
        m_menus.emplace_back( DebugMenu( "System", "System", [this] ( EntityUpdateContext const& context ) { DrawSystemMenu( context ); } ) );
    }

    void SystemDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        auto pSettingsRegistry = systemRegistry.GetSystem<SettingsRegistry>();
        auto const& debugSettings = pSettingsRegistry->GetAllDebugSettings();

        //-------------------------------------------------------------------------

        //// Create menu callbacks for all settings
        ////-------------------------------------------------------------------------


        //m_debugSettingMenuCallbacks.reserve( debugSettings.size() );

        //for ( auto& debugSetting : debugSettings )
        //{
        //    auto pSetting = debugSetting.second;
        //    auto drawFunction = [this, pSetting] ( UpdateContext const& context )
        //    {
        //        DrawSettingUI( pSetting );
        //    };

        //    //m_debugSettingMenuCallbacks.emplace_back( DebugMenu( pSetting->GetName(), pSetting->GetCategory(), drawFunction ) );
        //}

        //// Register all callbacks
        ////-------------------------------------------------------------------------

        //RegisterMenuCallbacks( m_debugSettingMenuCallbacks );
    }

    void SystemDebugView::Shutdown()
    {
        //UnregisterMenuCallbacks( m_debugSettingMenuCallbacks );
        //m_debugSettingMenuCallbacks.clear();
    }

    void SystemDebugView::DrawWindows( EntityUpdateContext const& context )
    {
        if ( m_isSystemLogWindowOpen )
        {
            DrawSystemLogWindow( context );
        }
    }

    void SystemDebugView::DrawSystemMenu( EntityUpdateContext const& context )
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

    void SystemDebugView::DrawSystemLogWindow( EntityUpdateContext const& context )
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

    void SystemDebugView::DrawDebugSetting( DebugSetting* pDebugSetting )
    {
        KRG_ASSERT( pDebugSetting != nullptr );

        //-------------------------------------------------------------------------

        switch ( pDebugSetting->GetType() )
        {
            case DebugSetting::Type::Bool:
            {
                auto pSetting = static_cast<DebugSettingBool*>( pDebugSetting );
                bool value = *pSetting;
                if ( ImGui::Checkbox( pDebugSetting->GetName(), &value ) )
                {
                    *pSetting = value;
                }
            }
            break;

            case DebugSetting::Type::Int:
            {
                auto pSetting = static_cast<DebugSettingInt*>( pDebugSetting );
                int32 value = *pSetting;

                if ( pSetting->HasLimits() )
                {
                    if ( ImGui::SliderInt( pDebugSetting->GetName(), &value, pSetting->GetMin(), pSetting->GetMax() ) )
                    {
                        *pSetting = value;
                    }
                }
                else
                {
                    if ( ImGui::InputInt( pDebugSetting->GetName(), &value ) )
                    {
                        *pSetting = value;
                    }
                }
            }
            break;

            case DebugSetting::Type::Float:
            {
                auto pSetting = static_cast<DebugSettingFloat*>( pDebugSetting );
                float value = *pSetting;

                if ( pSetting->HasLimits() )
                {
                    if ( ImGui::SliderFloat( pDebugSetting->GetName(), &value, pSetting->GetMin(), pSetting->GetMax() ) )
                    {
                        *pSetting = value;
                    }
                }
                else
                {
                    if ( ImGui::InputFloat( pDebugSetting->GetName(), &value, 0.1f, 1.0f ) )
                    {
                        *pSetting = value;
                    }
                }
            }
            break;
        }
    }
}
#endif