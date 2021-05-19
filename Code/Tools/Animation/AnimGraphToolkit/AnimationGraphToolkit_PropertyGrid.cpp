#include "AnimationGraphToolkit_PropertyGrid.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    GraphPropertyGrid::GraphPropertyGrid( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
        , m_propertyGrid( pModel )
    {}

    void GraphPropertyGrid::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "PropertyGrid" ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }
}