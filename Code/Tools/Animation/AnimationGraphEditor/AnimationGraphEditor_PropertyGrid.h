#pragma once
#include "Tools/Core/Editor/EditorTool.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphPropertyGrid final : public TEditorTool<GraphEditorModel>
    {
    public:

        GraphPropertyGrid( EditorModel* pModel );

        virtual char const* const GetName() { return "Property Grid"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        PropertyGrid    m_propertyGrid;
    };
}