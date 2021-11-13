#include "NumericEditors.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    static bool DrawVectorElement( char const* pID, char const* pLabel, float const& width, ImVec4 backgroundColor, float* pValue, bool isReadOnly = false )
    {
        constexpr static float const labelWidth = 16.0f;
        constexpr static float const labelHeight = 22.0f;

        //-------------------------------------------------------------------------

        bool result = false;

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, backgroundColor );

        ImGui::BeginChild( pLabel, ImVec2( width, labelHeight ), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::AlignTextToFramePadding();
            ImGui::SetCursorPosX( 4 );
            {
                ImGuiX::ScopedFont sf( Font::MediumBold );
                ImGui::Text( pLabel );
            }

            ImGui::SameLine( 0, 0 );
            ImGui::SetCursorPosX( labelWidth );
            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 1 );

            ImGui::SetNextItemWidth( width - labelWidth - 1 );
            ImGui::InputFloat( pID, pValue, 0, 0, "%.3f", isReadOnly ? ImGuiInputTextFlags_ReadOnly : 0 );
            result = ImGui::IsItemDeactivatedAfterEdit();
        }
        ImGui::EndChild();

        ImGui::PopStyleVar( 3 );
        ImGui::PopStyleColor();

        return result;
    }

    //-------------------------------------------------------------------------

    bool InputFloat2( char const* pID, Float2& value, float width, bool isReadOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - itemSpacing ) / 2;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            if ( DrawVectorElement( "##x", "X", inputWidth, Colors::Red.ToFloat4(), &value.m_x, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##y", "Y", inputWidth, Colors::Green.ToFloat4(), &value.m_y, isReadOnly ) )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }

    bool InputFloat3( char const* pID, Float3& value, float width, bool isReadOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( itemSpacing * 2 ) ) / 3;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            if ( DrawVectorElement( "##x", "X", inputWidth, Colors::Red.ToFloat4(), &value.m_x, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##y", "Y", inputWidth, Colors::Green.ToFloat4(), &value.m_y, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##z", "Z", inputWidth, Colors::RoyalBlue.ToFloat4(), &value.m_z, isReadOnly ) )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }

    bool InputFloat4( char const* pID, Float4& value, float width, bool isReadOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( itemSpacing * 3 ) ) / 4;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            if ( DrawVectorElement( "##x", "X", inputWidth, Colors::Red.ToFloat4(), &value.m_x, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##y", "Y", inputWidth, Colors::Green.ToFloat4(), &value.m_y, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##z", "Z", inputWidth, Colors::RoyalBlue.ToFloat4(), &value.m_z, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##w", "W", inputWidth, Colors::DarkOrange.ToFloat4(), &value.m_w, isReadOnly ) )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }

    bool InputFloat4( char const* pID, Vector& value, float width, bool isReadOnly )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( itemSpacing * 3 ) ) / 4;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pID );
        {
            if ( DrawVectorElement( "##x", "X", inputWidth, Colors::Red.ToFloat4(), &value.m_x, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##y", "Y", inputWidth, Colors::Green.ToFloat4(), &value.m_y, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##z", "Z", inputWidth, Colors::RoyalBlue.ToFloat4(), &value.m_z, isReadOnly ) )
            {
                valueUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( DrawVectorElement( "##w", "W", inputWidth, Colors::DarkOrange.ToFloat4(), &value.m_w, isReadOnly ) )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        return valueUpdated;
    }
}
#endif