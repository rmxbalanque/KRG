#pragma once
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphPropertyGrid
    {
    public:

        GraphPropertyGrid( GraphEditorModel& model );

        void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager );

    private:

        GraphEditorModel&       m_model;
        PropertyGrid            m_propertyGrid;
    };
}