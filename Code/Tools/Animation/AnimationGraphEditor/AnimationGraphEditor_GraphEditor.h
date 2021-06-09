#pragma once
#include "AnimationGraphEditor_Model.h"
#include "Tools/Core/Editor/EditorTool.h"
#include "Tools/Core/GraphEditor/FlowGraphView.h"

//-------------------------------------------------------------------------

struct ImNodesEditorContext;

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphEditorView final : public TEditorTool<GraphEditorModel>
    {
        enum class ViewID
        {
            Primary,
            Secondary
        };

        //-------------------------------------------------------------------------

        class GraphView : public GraphEditor::FlowGraphView
        {
        public:

            GraphView( GraphEditorModel const& model ) : m_model( model ) {}

        protected:

            virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos ) override;
            void DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, ToolsGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category );

        private:

            GraphEditorModel const&     m_model;
        };

    public:

        GraphEditorView( EditorModel* pModel );

        virtual char const* const GetName() { return "Graph Editor"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        void HandleFocusChanges();

    private:

        float                           m_primaryGraphViewHeight = 300;
        GraphView                       m_primaryFlowGraphView;
        GraphView                       m_secondaryFlowGraphView;
        ViewID                          m_lastFocusedGraph = ViewID::Primary;
    };
}