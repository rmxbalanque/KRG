#include "NumericEditors.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    static float const g_labelWidth = 16;

    static void DrawElementLabel( char const* pText, ImVec4 backgroundColor )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildBorderSize, 0.0f );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, backgroundColor );

        ImGui::BeginChild( pText, ImVec2( g_labelWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( pText );
        }
        ImGui::EndChild();

        ImGui::PopStyleVar( 2 );
        ImGui::PopStyleColor();
    }

    //-------------------------------------------------------------------------

    bool InputFloat2( char const* pID, Float2& value, float width, bool readOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( g_labelWidth * 2 ) - ( itemSpacing * 1 ) ) / 2;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            DrawElementLabel( " X", Colors::Red.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##x", &value.m_x, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " Y", Colors::Green.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##y", &value.m_y, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }

    bool InputFloat3( char const* pID, Float3& value, float width, bool readOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( g_labelWidth * 3 ) - ( itemSpacing * 2 ) ) / 3;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            DrawElementLabel( " X", Colors::Red.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##x", &value.m_x, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " Y", Colors::Green.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##y", &value.m_y, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " Z", Colors::CornflowerBlue.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##z", &value.m_z, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }

    bool InputFloat4( char const* pID, Float4& value, float width, bool readOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( g_labelWidth * 4 ) - ( itemSpacing * 3 ) ) / 4;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            DrawElementLabel( " X", Colors::Red.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##f4x", &value.m_x, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " Y", Colors::Green.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##f4y", &value.m_y, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " Z", Colors::CornflowerBlue.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##f4z", &value.m_z, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            DrawElementLabel( " W", Colors::Orange.ToFloat4() );

            ImGui::SameLine( 0, 0 );
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##f4w", &value.m_w, 0, 0, "%.3f", readOnly ? ImGuiInputTextFlags_ReadOnly : 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }
}
#endif