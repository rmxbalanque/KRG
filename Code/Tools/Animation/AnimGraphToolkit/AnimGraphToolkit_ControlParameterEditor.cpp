#include "AnimGraphToolkit_ControlParameterEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    GraphControlParameterEditor::GraphControlParameterEditor( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
    {}

    void GraphControlParameterEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Control Parameters" ) )
        {
            ImGui::Text( "TODO" );
        }
        ImGui::End();
    }
}