#include "ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    ImVec2 const& GetClosestPointOnRect( ImRect const& rect, ImVec2 const& inPoint )
    {
        ImVec2 const points[4] =
        {
            ImLineClosestPoint( rect.GetTL(), rect.GetTR(), inPoint ),
            ImLineClosestPoint( rect.GetBL(), rect.GetBR(), inPoint ),
            ImLineClosestPoint( rect.GetTL(), rect.GetBL(), inPoint ),
            ImLineClosestPoint( rect.GetTR(), rect.GetBR(), inPoint )
        };

        float distancesSq[4] =
        {
            ImLengthSqr( points[0] - inPoint ),
            ImLengthSqr( points[1] - inPoint ),
            ImLengthSqr( points[2] - inPoint ),
            ImLengthSqr( points[3] - inPoint )
        };

        // Get closest point
        float lowestDistance = FLT_MAX;
        int32 closestPointIdx = -1;
        for ( auto i = 0; i < 4; i++ )
        {
            if ( distancesSq[i] < lowestDistance )
            {
                closestPointIdx = i;
                lowestDistance = distancesSq[i];
            }
        }

        KRG_ASSERT( closestPointIdx >= 0 && closestPointIdx < 4 );
        return points[closestPointIdx];
    }
}

//-------------------------------------------------------------------------
// Separators
//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    static void CenteredSeparator( float width )
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if ( window->SkipItems )
        {
            return;
        }

        ImGuiContext& g = *GImGui;

        // Horizontal Separator
        float x1, x2;
        if ( window->DC.CurrentColumns == nullptr && ( width == 0 ) )
        {
            // Span whole window
            x1 = window->DC.CursorPos.x;
            x2 = x1 + window->Size.x;
        }
        else
        {
            // Start at the cursor
            x1 = window->DC.CursorPos.x;
            if ( width != 0 )
            {
                x2 = x1 + width;
            }
            else
            {
                x2 = window->ClipRect.Max.x;

                // Pad right side of columns (except the last one)
                if ( window->DC.CurrentColumns && ( window->DC.CurrentColumns->Current < window->DC.CurrentColumns->Count - 1 ) )
                {
                    x2 -= g.Style.ItemSpacing.x;
                }
            }
        }
        float y1 = window->DC.CursorPos.y + int( window->DC.CurrLineSize.y / 2.0f );
        float y2 = y1 + 1.0f;

        window->DC.CursorPos.x += width; //+ g.Style.ItemSpacing.x;
        x1 += window->DC.GroupOffset.x;

        const ImRect bb( ImVec2( x1, y1 ), ImVec2( x2, y2 ) );
        ImGui::ItemSize( ImVec2( 0.0f, 0.0f ) ); // NB: we don't provide our width so that it doesn't get feed back into AutoFit, we don't provide height to not alter layout.
        if ( !ImGui::ItemAdd( bb, NULL ) )
        {
            return;
        }

        window->DrawList->AddLine( bb.Min, ImVec2( bb.Max.x, bb.Min.y ), ImGui::GetColorU32( ImGuiCol_Separator ) );
    }

    void PostSeparator( float width )
    {
        ImGui::SameLine();
        CenteredSeparator( width );
    }

    void PreSeparator( float width )
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if ( window->DC.CurrLineSize.y == 0 )
        {
            window->DC.CurrLineSize.y = ImGui::GetTextLineHeight();
        }
        CenteredSeparator( width );
        ImGui::SameLine();
    }

    void TextSeparator( char const* text, float preWidth, float totalWidth )
    {
        PreSeparator( preWidth );
        ImGui::Text( text );

        // If we have a total width specified, calculate the post separator width
        if ( totalWidth != 0 )
        {
            totalWidth = totalWidth - ( ImGui::CalcTextSize( text ).x + preWidth + ( ImGui::GetStyle().ItemSpacing.x * 2 ) );
        }
        PostSeparator( totalWidth );
    }

    void VerticalSeparator( ImVec2 const& size, ImColor const& color )
    {
        ImGui::SameLine(0, 1);

        //-------------------------------------------------------------------------

        auto const canvasPos = ImGui::GetCursorScreenPos();
        auto const availableRegion = ImGui::GetContentRegionAvail();

        ImVec2 const seperatorSize( size.x <= 0 ? 1 : size.x, size.y <= 0 ? availableRegion.y : size.y );
        ImGui::SameLine( 0, seperatorSize.x + 2 );

        //-------------------------------------------------------------------------

        ImColor const separatorColor = ( (int) color == 0 ) ? ImGui::GetStyleColorVec4( ImGuiCol_Separator ) : ImColor( color );

        float const startPosX = Math::Floor( canvasPos.x - 2 + Math::Floor( seperatorSize.x / 2 ) );
        float const startPosY = canvasPos.y + 1;
        float const endPosY = startPosY + seperatorSize.y - 2;

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddLine( ImVec2( startPosX, startPosY ), ImVec2( startPosX, endPosY ), separatorColor, 1 );
    }
}

//-------------------------------------------------------------------------
// Widgets
//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    bool ColoredButton( ImColor const& backgroundColor, ImColor const& foregroundColor, char const* label, ImVec2 const& size )
    {
        ImVec4 const activeColor = (ImVec4) AdjustColorBrightness( backgroundColor, 1.2f );

        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) backgroundColor );
        ImGui::PushStyleColor( ImGuiCol_ButtonHovered, activeColor );
        ImGui::PushStyleColor( ImGuiCol_ButtonActive, activeColor );
        ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) foregroundColor );
        bool const result = ImGui::Button( label, size );
        ImGui::PopStyleColor( 4 );

        return result;
    }

    void DrawArrow( ImDrawList* pDrawList, ImVec2 const& arrowStart, ImVec2 const& arrowEnd, ImU32 col, float arrowWidth, float arrowHeadWidth )
    {
        KRG_ASSERT( pDrawList != nullptr );

        ImVec2 const direction = Vector( arrowEnd - arrowStart ).GetNormalized2();
        ImVec2 const orthogonalDirection( -direction.y, direction.x );

        ImVec2 const triangleSideOffset = orthogonalDirection * arrowHeadWidth;
        ImVec2 const triBase = arrowEnd - ( direction * arrowHeadWidth );
        ImVec2 const tri1 = triBase - triangleSideOffset;
        ImVec2 const tri2 = triBase + triangleSideOffset;

        pDrawList->AddLine( arrowStart, triBase, col, arrowWidth );
        pDrawList->AddTriangleFilled( arrowEnd, tri1, tri2, col );
    }

    bool DrawOverlayIcon( ImVec2 const& iconPos, char icon[4], void* iconID )
    {
        bool result = false;

        //-------------------------------------------------------------------------

        static ImVec2 const iconSize( 48, 48 );
        ImVec2 const iconHalfSize( iconSize.x / 2, iconSize.y / 2 );
        ImRect const iconRect( iconPos - iconHalfSize, iconPos + iconHalfSize );
        ImRect const windowRect( ImVec2( 0, 0 ), ImGui::GetWindowSize() );
        if ( !windowRect.Overlaps( iconRect ) )
        {
            return result;
        }

        //-------------------------------------------------------------------------

        ImGuiX::ScopedFont scopedFont( ImGuiX::Font::Huge );
        ImVec2 const textSize = ImGui::CalcTextSize( icon );
        ImGui::SetCursorPos( iconPos - iconHalfSize );
        ImGui::PushID( iconID );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0, 0, 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0, 0, 0, 0 ) );
        if ( ImGui::Button( icon, iconSize ) )
        {
            result = true;
        }
        ImGui::PopStyleColor( 3 );
        ImGui::PopStyleVar( 1 );
        ImGui::PopID();

        return result;
    }

    bool DrawSpinner( const char* pLabel, ImColor const& color, float radius, float thickness )
    {
        static float const numSegments = 30.0f;

        //-------------------------------------------------------------------------

        ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
        if ( pWindow->SkipItems )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        ImGuiStyle const& style = ImGui::GetStyle();
        ImGuiID const id = pWindow->GetID( pLabel );

        ImVec2 const pos = pWindow->DC.CursorPos;
        ImVec2 const size( ( radius ) * 2, ( radius + style.FramePadding.y ) * 2 );

        ImRect const bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
        ImGui::ItemSize( bb, style.FramePadding.y );
        if ( !ImGui::ItemAdd( bb, id ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        pWindow->DrawList->PathClear();

        float const time = (float) ImGui::GetTime();
        float const start = Math::Abs( Math::Sin( time * 1.8f ) * ( numSegments - 5 ) );
        float const min = Math::Pi * 2.0f * ( start ) / numSegments;
        float const max = Math::Pi * 2.0f * ( numSegments - 3 ) / numSegments;

        ImVec2 const center = ImVec2( pos.x + radius, pos.y + radius + style.FramePadding.y );

        for ( float i = 0; i < numSegments; i++ )
        {
            float const a = min + ( i / numSegments ) * ( max - min );
            float const b = a + ( time * 8 );
            pWindow->DrawList->PathLineTo( ImVec2( center.x + Math::Cos( b ) * radius, center.y + Math::Sin( b ) * radius ) );
        }

        pWindow->DrawList->PathStroke( color, false, thickness );

        return true;
    }
}

//-------------------------------------------------------------------------
// Numeric Helpers
//-------------------------------------------------------------------------

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

        if ( BeginElementFrame( pLabel, width, backgroundColor ) )
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
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const inputWidth = Math::Floor( ( contentWidth - itemSpacing ) / 2 );

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
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const inputWidth = Math::Floor( ( contentWidth - ( itemSpacing * 2 ) ) / 3 );

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
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const inputWidth = Math::Floor( ( contentWidth - ( itemSpacing * 3 ) ) / 4 );

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
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
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
        return ImGui::BeginChild( "Contents", ImVec2( ImGui::GetContentRegionAvail().x - 2, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
    }

    static void EndItemFrame()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar( 3 );
        ImGui::PopStyleColor();
    }

    void DisplayVector2( ImGuiID ID, Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const elementWidth = Math::Floor( ( contentWidth - itemSpacing ) / 2 );

        ImGui::PushID( ID );
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

    void DisplayVector3( ImGuiID ID, Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const elementWidth = Math::Floor( ( contentWidth - itemSpacing ) / 3 );

        ImGui::PushID( ID );
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

    void DisplayVector4( ImGuiID ID, Vector const& v, float width )
    {
        float const contentWidth = ( width > 0 ) ? width : ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const elementWidth = Math::Floor( ( contentWidth - itemSpacing ) / 4 );

        ImGui::PushID( ID );
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

    void DisplayRotation( ImGuiID ID, Quaternion const& q, float width )
    {
        Float3 const rotation = q.ToEulerAngles().GetAsDegrees();
        DisplayVector3( ID, rotation, width );
    }

    void DisplayTransform( ImGuiID ID, Transform const& t, float width )
    {
        constexpr float const tableLabelRowWidth = 24;

        ImGui::PushID( ID );
        if ( ImGui::BeginTable( "Transform", 2, 0, ImVec2( width, 0 ) ) )
        {
            ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, tableLabelRowWidth );
            ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Rot" );

            ImGui::TableNextColumn();
            DisplayRotation( t.GetRotation(), ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Pos" );

            ImGui::TableNextColumn();
            DisplayVector3( t.GetTranslation(), ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Scl" );

            ImGui::TableNextColumn();
            DisplayVector3( t.GetScale(), ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x );

            ImGui::EndTable();
        }
        ImGui::PopID();
    }
}