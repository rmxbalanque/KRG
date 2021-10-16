#include "AnimationGraphEditor_PropertyGrid.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphPropertyGrid::GraphPropertyGrid( GraphEditorModel& graphModel )
        : m_graphModel( graphModel )
        , m_propertyGrid( *graphModel.GetTypeRegistry(), graphModel.GetSourceDataDirectory() )
    {}

    void GraphPropertyGrid::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        auto const& selection = m_graphModel.GetSelectedNodes();

        if ( selection.empty() )
        {
            m_propertyGrid.SetTypeToEdit( nullptr );
        }
        else
        {
            m_propertyGrid.SetTypeToEdit( selection.back() );
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }
}