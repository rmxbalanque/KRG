#pragma once
#include "Tools/Editor/EditorTool.h"
#include "Tools/Editor/Widgets/PropertyGrid/PropertyGrid.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class GraphPropertyGrid final : public TEditorTool<EditorModel>
    {
    public:

        GraphPropertyGrid( EditorModel* pModel );

        virtual char const* const GetName() { return "Property Grid"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Editor::PropertyGrid    m_propertyGrid;
    };
}