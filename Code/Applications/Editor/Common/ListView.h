#pragma once

#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ListViewRow
    {
    public:

        virtual void Draw() const {}
    };

    class TestRow : public ListViewRow
    {
    public:

        TestRow( String const& str ) : m_text( str ) {}

        virtual void Draw() const { ImGui::Text( m_text.c_str() ); }

        String m_text;
    };

    //-------------------------------------------------------------------------

    template<class T>
    class ListView
    {
        static_assert( std::is_base_of<ListViewRow, T>::value, "T must derive from ListViewRow" );

    public:

        ListView();

        void Draw();

    public:

        TVector<T>                  m_rows;
        T*                          m_pSelectedRow = nullptr;
        float                       m_rowHeight = -1.0f;
        float                       m_totalHeight = -1.0f;
    };

    //-------------------------------------------------------------------------

    template<class T>
    ListView<T>::ListView()
    {
        m_rows.reserve( 10000 );
        String str;
        for ( auto i = 0; i < 10000; i++ )
        {
            m_rows.emplace_back( TestRow( str.sprintf( "list item %d", i ) ) );
        }
    }

    template<class T>
    void ListView<T>::Draw()
    {
        if ( m_rows.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::BeginChild( "ListView", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );
        {
            float const totalVerticalSpaceAvailable = ImGui::GetContentRegionAvail().y;
            float const currentVerticalScrollPosition = ImGui::GetScrollY();

            //-------------------------------------------------------------------------

            float firstElementOffset = 0;
            if ( m_totalHeight < 0 )
            {
                m_totalHeight = 0;

                // Draw first row to get row height (assumes uniform row height)
                ImVec2 const prevCursorPos = ImGui::GetCursorPos();
                m_rows[0].Draw();
                ImVec2 const cursorPos = ImGui::GetCursorPos();
                m_rowHeight = cursorPos.y - prevCursorPos.y;
                m_totalHeight = m_rowHeight * m_rows.size();
                firstElementOffset = m_rowHeight;
            }

            //-------------------------------------------------------------------------

            int32 const itemsToDrawStartIdx = (int32) Math::Floor( currentVerticalScrollPosition / m_rowHeight );
            int32 const numItemsToDraw = (int32) Math::Ceiling( totalVerticalSpaceAvailable / m_rowHeight );
            int32 const itemsToDrawEndIdx = Math::Min( (int32) m_rows.size() - 1, itemsToDrawStartIdx + numItemsToDraw );

            ImGui::Dummy( ImVec2( -1, currentVerticalScrollPosition - firstElementOffset ) );

            for ( int32 i = itemsToDrawStartIdx; i <= itemsToDrawEndIdx; i++ )
            {
                m_rows[i].Draw();
            }

            ImGui::Dummy( ImVec2( -1, m_totalHeight - totalVerticalSpaceAvailable - currentVerticalScrollPosition ) );
        }
        ImGui::EndChild();
        ImGui::PopID();
    }
}