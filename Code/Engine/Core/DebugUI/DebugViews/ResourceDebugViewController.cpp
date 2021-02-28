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
    }

    //-------------------------------------------------------------------------

    void ResourceDebugViewController::DrawResourceMenu( UpdateContext const& context )
    {
        if ( ImGui::Button( "Show Request History" ) )
        {
            m_isHistoryWindowOpen = true;
        }
    }

    void ResourceDebugViewController::DrawHistoryWindow( UpdateContext const& context )
    {
        KRG_ASSERT( m_isHistoryWindowOpen );

        ImGui::SetNextWindowBgAlpha( 0.75f );
        if ( ImGui::Begin( "Resource Request History", &m_isHistoryWindowOpen ) )
        {
            if ( ImGui::BeginTable( "Resource History Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Time", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 50 );
                ImGui::TableSetupColumn( "Request", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 45 );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30 );
                ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto i = (S32) m_pResourceSystem->m_history.size() - 1; i >= 0; i-- )
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

            ImGui::End();
        }
    }
}
#endif