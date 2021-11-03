#pragma once
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphPropertyGrid
    {
    public:

        GraphPropertyGrid( GraphEditorModel& graphModel );

        void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName );

    private:

        GraphEditorModel&           m_graphModel;
        PropertyGrid                m_propertyGrid;
    };
}