#include "AnimationGraphEditor_PropertyGrid.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphPropertyGrid::GraphPropertyGrid( EditorModel* pModel )
        : TEditorTool<GraphEditorModel>( pModel )
        , m_propertyGrid( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {}

    void GraphPropertyGrid::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto const& selection = m_model.GetSelectedNodes();
        if ( selection.empty() )
        {
            m_propertyGrid.SetTypeToEdit( nullptr );
        }
        else
        {
            m_propertyGrid.SetTypeToEdit( selection.back() );
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "PropertyGrid" ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }
}