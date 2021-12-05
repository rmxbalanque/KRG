#include "NumericUIHelpers.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    constexpr static float const g_labelWidth = 16.0f;
    constexpr static float const g_labelHeight = 24.0f;

    static bool BeginElementFrame( char const* pLabel, float const& width, ImVec4 backgroundColor )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, backgroundColor );

        if ( ImGui::BeginChild( pLabel, ImVec2( width, g_labelHeight ), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
        {
            ImGui::AlignTextToFramePadding();
            ImGui::SetCursorPosX( 4 );
            {
                ImGuiX::ScopedFont sf( Font::MediumBold );
                ImGui::Text( pLabel );
            }

            ImGui::SameLine( 0, 0 );
            ImGui::SetCursorPosX( g_labelWidth );
            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 1 );

            return true;
        }

        return false;
    }

    static void EndElementFrame()
    {
        ImGui::EndChild();

        ImGui::PopStyleVar( 3 );
        ImGui::PopStyleColor();
    }

    static bool DrawVectorElement( char const* pID, char const* pLabel, float const& width, ImVec4 backgroundColor, float* pValue, bool isReadOnly = false )
    {
        bool result = false;

        if( BeginElementFrame( pLabel, width, backgroundColor ) )
        {
            ImGui::SetNextItemWidth( width - g_labelWidth - 1 );
            ImGui::InputFloat( pID, pValue, 0, 0, "%.3f", isReadOnly ? ImGuiInputTextFlags_ReadOnly : 0 );
            result = ImGui::IsItemDeactivatedAfterEdit();
        }
        EndElementFrame();

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

    //-------------------------------------------------------------------------

    static bool BeginItemFrame()
    {
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 3 ) );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, (ImVec4) ImGuiX::Style::s_itemColorDark );
        return ImGui::BeginChild( "Contents", ImVec2( ImGui::GetContentRegionAvail().x - 3, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
    }

    static void EndItemFrame()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar( 3 );
        ImGui::PopStyleColor();
    }

    void DisplayVector2( Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const elementWidth = ( contentWidth - itemSpacing ) / 2;

        ImGui::PushID( &v );
        if ( BeginElementFrame( "X", elementWidth, Colors::Red.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetX() );
            }
            EndItemFrame();
           
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "Y", elementWidth, Colors::Green.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetY() );
            }
            EndItemFrame();
        }
        EndElementFrame();
        ImGui::PopID();
    }

    void DisplayVector3( Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const elementWidth = ( contentWidth - itemSpacing ) / 3;

        ImGui::PushID( &v );
        if ( BeginElementFrame( "X", elementWidth, Colors::Red.ToFloat4() ) )
        { 
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetX() );
            }
            EndItemFrame();
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "Y", elementWidth, Colors::Green.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetY() );
            }
            EndItemFrame();
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "Z", elementWidth, Colors::RoyalBlue.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetZ() );
            }
            EndItemFrame();
        }
        EndElementFrame();
        ImGui::PopID();
    }

    void DisplayVector4( Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const elementWidth = ( contentWidth - itemSpacing ) / 4;

        ImGui::PushID( &v );
        if ( BeginElementFrame( "X", elementWidth, Colors::Red.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetX() );
            }
            EndItemFrame();
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "Y", elementWidth, Colors::Green.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetY() );
            }
            EndItemFrame();
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "Z", elementWidth, Colors::RoyalBlue.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetZ() );
            }
            EndItemFrame();
        }
        EndElementFrame();

        ImGui::SameLine( 0, itemSpacing );

        if ( BeginElementFrame( "W", elementWidth, Colors::DarkOrange.ToFloat4() ) )
        {
            if ( BeginItemFrame() )
            {
                ImGui::Text( "%.3f", v.GetW() );
            }
            EndItemFrame();
        }
        EndElementFrame();
        ImGui::PopID();
    }

    void DisplayRotation( Quaternion const& q, float width )
    {
        Float3 const rotation = q.ToEulerAngles().GetAsDegrees();
        DisplayVector3( rotation, width );
    }

    void DisplayTransform( Transform const& t, float width )
    {
        constexpr float const tableLabelRowWidth = 24;

        if ( ImGui::BeginTable( "Transform", 2, 0, ImVec2( width, 0 ) ) )
        {
            ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, tableLabelRowWidth );
            ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Rot" );

            ImGui::TableNextColumn();
            DisplayRotation( t.GetRotation(), width - tableLabelRowWidth );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Pos" );

            ImGui::TableNextColumn();
            DisplayVector3( t.GetTranslation(), width - tableLabelRowWidth );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Scl" );

            ImGui::TableNextColumn();
            DisplayVector3( t.GetScale(), width - tableLabelRowWidth );

            ImGui::EndTable();
        }
    }
}
#endif