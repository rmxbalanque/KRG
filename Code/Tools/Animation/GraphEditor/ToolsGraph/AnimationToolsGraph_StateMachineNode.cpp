#include "AnimationToolsGraph_StateMachineNode.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Common.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_StateBaseNode::DrawExtraControls( GraphEditor::DrawContext const& ctx )
    {
        // State events
        //-------------------------------------------------------------------------

        InlineString<255> string;
        auto CreateEventString = [&] ( TVector<StringID> const& IDs )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) IDs.size(); i++ )
            {
                string += IDs[i].c_str();

                if ( i != IDs.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        auto CreateTimedEventString = [&] ( TVector<TimedStateEvent> const& events )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) events.size(); i++ )
            {
                string += events[i].m_ID.c_str();

                if ( i != events.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        CreateEventString( m_entryEvents );
        ImGui::Text( "Entry: %s", string.c_str() );

        CreateEventString( m_executeEvents );
        ImGui::Text( "Execute: %s", string.c_str() );

        CreateEventString( m_exitEvents );
        ImGui::Text( "Exit: %s", string.c_str() );

        CreateTimedEventString( m_timeRemainingEvents );
        ImGui::Text( "Time Left: %s", string.c_str() );

        CreateTimedEventString( m_timeElapsedEvents );
        ImGui::Text( "Time Elapsed: %s", string.c_str() );

        // Draw separator
        //-------------------------------------------------------------------------

        ImVec2 const originalCursorPos = ImGui::GetCursorScreenPos();
        float const width = Math::Max( GetSize().x, 40.0f );
        ImGui::InvisibleButton( "Spacer", ImVec2( width, 10 ) );
        ctx.m_pDrawList->AddLine( originalCursorPos + ImVec2( 0, 4 ), originalCursorPos + ImVec2( GetSize().x, 4 ), ImColor( ImGuiX::Style::s_itemColorLight ) );

        // Draw runtime debug info
        //-------------------------------------------------------------------------

        bool shouldDrawEmptyDebugInfoBlock = true;
        auto pDebugContext = reinterpret_cast<DebugContext*>( ctx.m_pUserContext );
        if ( pDebugContext != nullptr )
        {
            NodeIndex runtimeNodeIdx = pDebugContext->GetRuntimeNodeIndex( GetID() );
            if ( runtimeNodeIdx != InvalidIndex && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                PoseNodeDebugInfo const debugInfo = pDebugContext->GetPoseNodeDebugInfo( runtimeNodeIdx );
                DrawPoseNodeDebugInfo( ctx, GetSize().x, debugInfo );
                shouldDrawEmptyDebugInfoBlock = false;
            }
        }

        if ( shouldDrawEmptyDebugInfoBlock )
        {
            DrawEmptyPoseNodeDebugInfo( ctx, GetSize().x );
        }
    }
}