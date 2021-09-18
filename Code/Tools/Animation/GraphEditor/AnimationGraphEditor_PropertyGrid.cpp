#include "AnimationGraphEditor_PropertyGrid.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphPropertyGrid::GraphPropertyGrid( GraphEditorModel& model )
        : m_model( model )
        , m_propertyGrid( *model.GetTypeRegistry(), model.GetSourceDataDirectory() )
    {}

    void GraphPropertyGrid::Draw( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
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