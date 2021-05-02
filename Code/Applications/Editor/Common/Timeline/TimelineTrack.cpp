#include "TimelineTrack.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    Track::~Track()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void Track::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( GetLabel() );
    }
}