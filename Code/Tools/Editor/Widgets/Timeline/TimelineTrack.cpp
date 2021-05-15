#include "TimelineTrack.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Editor
{
    TimelineTrack::~TimelineTrack()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void TimelineTrack::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( GetLabel() );
    }
}