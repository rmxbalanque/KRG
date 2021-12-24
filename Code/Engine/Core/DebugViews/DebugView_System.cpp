#include "DebugView_System.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Profiling/Profiling.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    SystemDebugView::SystemDebugView()
    {
        m_menus.emplace_back( DebugMenu( "System", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void SystemDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        if ( ImGui::MenuItem( "Open Profiler" ) )
        {
            Profiling::OpenProfiler();
        }
    }

    //-------------------------------------------------------------------------

    SystemLogView::SystemLogView()
    {
        m_logFilter.resize( 255 );
    }

    bool SystemLogView::Draw( UpdateContext const& context )
    {
        bool isLogWindowOpen = true;

        if ( ImGui::Begin( "System Log", &isLogWindowOpen ) )
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Filter:" );
            ImGui::SameLine();
            ImGui::BeginDisabled();
            if ( ImGui::InputText( "##Filter", m_logFilter.data(), 255 ) )
            {
                // TODO
            }
            ImGui::EndDisabled();

            //-------------------------------------------------------------------------

            ImGui::SameLine();
            ImGui::Checkbox( "Messages", &m_showLogMessages );
            ImGui::SameLine();
            ImGui::Checkbox( "Warnings", &m_showLogWarnings );
            ImGui::SameLine();
            ImGui::Checkbox( "Errors", &m_showLogErrors );

            //-------------------------------------------------------------------------

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
                    switch ( entry.m_severity )
                    {
                        case Log::Severity::Warning:
                        if ( !m_showLogWarnings )
                        {
                            continue;
                        }
                        break;

                        case Log::Severity::Error:
                        if ( !m_showLogErrors )
                        {
                            continue;
                        }
                        break;

                        case Log::Severity::Message:
                        if ( !m_showLogMessages )
                        {
                            continue;
                        }
                        break;
                    }

                    //-------------------------------------------------------------------------

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

        //-------------------------------------------------------------------------

        return isLogWindowOpen;
    }

    //-------------------------------------------------------------------------

    static void DrawDebugSetting( DebugSetting* pDebugSetting )
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

    bool DebugSettingsView::Draw( UpdateContext const& context )
    {
        auto pSettingsRegistry = context.GetSystem<SettingsRegistry>();
        auto const& debugSettings = pSettingsRegistry->GetAllDebugSettings();

        //-------------------------------------------------------------------------
        
        bool isDebugSettingsWindowOpen = true;

        if ( ImGui::Begin( "Debug Settings", &isDebugSettingsWindowOpen ) )
        {
            if ( ImGui::BeginTable( "Settings Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Channel", ImGuiTableColumnFlags_WidthFixed, 200 );
                ImGui::TableSetupColumn( "Setting", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto const& settingPair : debugSettings )
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( settingPair.second->GetCategory() );

                    ImGui::TableSetColumnIndex( 1 );
                    DrawDebugSetting( settingPair.second );
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();

        //-------------------------------------------------------------------------

        return isDebugSettingsWindowOpen;
    }
}
#endif