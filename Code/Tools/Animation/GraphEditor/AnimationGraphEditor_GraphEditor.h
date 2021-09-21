#pragma once
#include "AnimationGraphEditor_Model.h"
#include "Tools/Core/GraphEditor/Flow/FlowGraphView.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraphView.h"

//-------------------------------------------------------------------------

struct ImNodesEditorContext;

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphEditorView
    {
        enum ViewID
        {
            None = -1,
            PrimaryFlow,
            PrimarySM,
            Secondary
        };

        //-------------------------------------------------------------------------

        class FlowGraphView : public GraphEditor::FlowGraphView
        {
        public:

            FlowGraphView( GraphEditorModel& model ) : m_model( model ) {}

        protected:

            virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos ) override;
            virtual void OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph ) override;
            virtual void OnNodeDoubleClick( GraphEditor::BaseNode* pNode ) override;
            virtual void OnSelectionChanged( TVector<GraphEditor::BaseNode*> const& oldSelection, TVector<GraphEditor::BaseNode*> const& newSelection ) { m_selectionChanged = true; }

            void DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category );

        public:

            GraphEditorModel&       m_model;
            bool                    m_selectionChanged = false;
            bool                    m_wasFocused = false;
        };

        //-------------------------------------------------------------------------

        class StateMachineGraphView : public GraphEditor::StateMachineGraphView
        {
        public:

            StateMachineGraphView( GraphEditorModel& model ) : m_model( model ) {}

        protected:

            virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos ) override;
            virtual void DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, GraphEditor::SM::Node* pNode ) override;
            virtual void OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph ) override;
            virtual void OnNodeDoubleClick( GraphEditor::BaseNode* pNode ) override;
            virtual void OnSelectionChanged( TVector<GraphEditor::BaseNode*> const& oldSelection, TVector<GraphEditor::BaseNode*> const& newSelection ) { m_selectionChanged = true; }
            virtual void DrawExtraInformation( GraphEditor::DrawingContext const& ctx ) override;

        public:

            GraphEditorModel&       m_model;
            bool                    m_selectionChanged = false;
            bool                    m_wasFocused = false;
        };

    public:

        GraphEditorView( GraphEditorModel& model );

        void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager );

        void RefreshView();

    private:

        void UpdatePrimaryViewState();
        void UpdateSecondaryViewState();
        void HandleFocusAndSelectionChanges();

    private:

        GraphEditorModel&               m_model;
        float                           m_primaryGraphViewHeight = 300;

        FlowGraphView                   m_primaryFlowGraphView;
        StateMachineGraphView           m_primaryStateMachineGraphView;
        FlowGraphView                   m_secondaryFlowGraphView;

        GraphEditor::BaseGraphView*     m_pPrimaryGraphView = &m_primaryFlowGraphView;
        GraphEditor::BaseGraphView*     m_pFocusedGraphView = nullptr;
    };
}