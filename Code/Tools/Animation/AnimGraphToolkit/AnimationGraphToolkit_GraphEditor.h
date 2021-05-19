#pragma once
#include "Tools/Editor/EditorTool.h"

//-------------------------------------------------------------------------

struct ImNodesEditorContext;

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class GraphEditor final : public TEditorTool<EditorModel>
    {
    public:

        GraphEditor( EditorModel* pModel );

        virtual char const* const GetName() { return "Graph Editor"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        void DrawPrimaryGraph();
        void DrawSecondaryGraph();

    private:

        float                       m_primaryGraphViewHeight = 300;
        ImNodesEditorContext*       m_pPrimaryGraphContext = nullptr;
        ImNodesEditorContext*       m_pSecondaryGraphContext = nullptr;
    };
}