#pragma once
#include "Tools/Editor/EditorTool.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class GraphControlParameterEditor final : public TEditorTool<EditorModel>
    {
    public:

        GraphControlParameterEditor( EditorModel* pModel );

        virtual char const* const GetName() { return "Control Parameters"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}