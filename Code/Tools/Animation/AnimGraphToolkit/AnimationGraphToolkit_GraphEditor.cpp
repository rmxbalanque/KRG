#include "AnimationGraphToolkit_GraphEditor.h"
#include "System/DevTools/ThirdParty/imnodes/imnodes.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    GraphEditor::GraphEditor( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
    {}

    void GraphEditor::Initialize( UpdateContext const& context )
    {
        TEditorTool<EditorModel>::Initialize( context );

        m_pPrimaryGraphContext = ImNodes::EditorContextCreate();
        m_pSecondaryGraphContext = ImNodes::EditorContextCreate();
    }

    void GraphEditor::Shutdown( UpdateContext const& context )
    {
        ImNodes::EditorContextFree( m_pPrimaryGraphContext );
        ImNodes::EditorContextFree( m_pSecondaryGraphContext );

        TEditorTool<EditorModel>::Shutdown( context );
    }

    void GraphEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::Begin( "Graph Editor" ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
            ImGui::BeginChild( "PrimaryGraphView", ImVec2( 0, m_primaryGraphViewHeight ), false );
            {
                DrawPrimaryGraph();
            }
            ImGui::EndChild();

            //-------------------------------------------------------------------------
            
            ImGui::InvisibleButton( "GraphViewSplitter", ImVec2( -1, 4.0f ) );

            if ( ImGui::IsItemHovered() )
            {
                ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNS );
            }

            if ( ImGui::IsItemActive() )
            {
                m_primaryGraphViewHeight += ImGui::GetIO().MouseDelta.y;
            }

            //-------------------------------------------------------------------------

            ImGui::BeginChild( "SecondaryGraphView", ImVec2( 0, 0 ), false );
            {
                DrawSecondaryGraph();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GraphEditor::DrawPrimaryGraph()
    {
        ImNodes::EditorContextSet( m_pPrimaryGraphContext );

        ImNodes::BeginNodeEditor();

        ImNodes::BeginNode( 1 );
        {
            ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted( "input node 2" );
            ImNodes::EndNodeTitleBar();

             ImNodes::BeginInputAttribute( 122 );
             ImGui::Text( "input pin 2" );
             ImNodes::EndInputAttribute();
        }
        ImNodes::EndNode();

        ImNodes::BeginNode( 2 );
        {
            ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted( "output node 2 " );
            ImNodes::EndNodeTitleBar();

            ImNodes::BeginOutputAttribute( 12 );
            ImGui::Text( "output pin 2" );
            ImNodes::EndOutputAttribute();
        }
        ImNodes::EndNode();

        //-------------------------------------------------------------------------

        ImNodes::Link( 2134, 12, 122 );

        //-------------------------------------------------------------------------

        ImNodes::EndNodeEditor();
    }

    void GraphEditor::DrawSecondaryGraph()
    {
        ImNodes::EditorContextSet( m_pSecondaryGraphContext );

        ImNodes::BeginNodeEditor();

        ImNodes::BeginNode( 545 );
        {
            ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted( "input node" );
            ImNodes::EndNodeTitleBar();

            ImNodes::BeginInputAttribute( 122 );
            ImGui::Text( "input pin" );
            ImNodes::EndInputAttribute();
        }
        ImNodes::EndNode();

        ImNodes::BeginNode( 2 );
        {
            ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted( "output node" );
            ImNodes::EndNodeTitleBar();

            ImNodes::BeginOutputAttribute( 12 );
            ImGui::Text( "output pin" );
            ImNodes::EndOutputAttribute();
        }
        ImNodes::EndNode();

        //-------------------------------------------------------------------------

        ImNodes::Link( 2134, 12, 122 );

        //-------------------------------------------------------------------------

        ImNodes::EndNodeEditor();
    }
}