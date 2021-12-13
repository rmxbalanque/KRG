#pragma once
#include "Tools/Core/GraphEditor/Flow/FlowGraphView.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraphView.h"
#include "Tools/Core/Trees/CategoryTree.h"

//-------------------------------------------------------------------------

struct ImNodesEditorContext;
namespace KRG { class UpdateContext; }
namespace KRG::Animation::Graph
{
    class FlowGraph; 
    class AnimationGraphToolsDefinition;
    struct DebugContext;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphView
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

            FlowGraphView( GraphView& parentGraphView ) : m_parentGraphView( parentGraphView ) {}

        protected:

            virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos ) override;
            virtual void OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph ) override;
            virtual void OnNodeDoubleClick( GraphEditor::BaseNode* pNode ) override;
            virtual void OnSelectionChanged( TVector<GraphEditor::BaseNode*> const& oldSelection, TVector<GraphEditor::BaseNode*> const& newSelection ) { m_selectionChanged = true; }

            void DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category );

        public:

            GraphView&              m_parentGraphView;
            bool                    m_selectionChanged = false;
            bool                    m_wasFocused = false;
        };

        //-------------------------------------------------------------------------

        class StateMachineGraphView : public GraphEditor::StateMachineGraphView
        {
        public:

            StateMachineGraphView( GraphView& parentGraphView ) : m_parentGraphView( parentGraphView ) {}

        protected:

            virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos ) override;
            virtual void DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, GraphEditor::SM::Node* pNode ) override;
            virtual void OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph ) override;
            virtual void OnNodeDoubleClick( GraphEditor::BaseNode* pNode ) override;
            virtual void OnSelectionChanged( TVector<GraphEditor::BaseNode*> const& oldSelection, TVector<GraphEditor::BaseNode*> const& newSelection ) { m_selectionChanged = true; }
            virtual void DrawExtraInformation( GraphEditor::DrawContext const& ctx ) override;

        public:

            GraphView&              m_parentGraphView;
            bool                    m_selectionChanged = false;
            bool                    m_wasFocused = false;
        };

    public:

        GraphView( TypeSystem::TypeRegistry const& typeRegistry, AnimationGraphToolsDefinition* pToolsGraph );

        // Graph information
        AnimationGraphToolsDefinition* GetGraphDefinition() { return m_pGraphDefinition; }
        AnimationGraphToolsDefinition const* GetGraphDefinition() const { return m_pGraphDefinition; }
        inline Category<TypeSystem::TypeInfo const*> const& GetNodeTypes() const { return m_categorizedNodeTypes.GetRootCategory(); }

        // Update
        void UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName );
        void OnUndoRedo();

        // Navigation
        void NavigateTo( GraphEditor::BaseNode* pNode );
        void NavigateTo( GraphEditor::BaseGraph* pGraph );

        // Selection
        TVector<GraphEditor::BaseGraphView::SelectedNode> const& GetSelectedNodes() const;
        void ClearSelection();

    private:

        void UpdateSecondaryViewState();
        void HandleFocusAndSelectionChanges();

    private:

        AnimationGraphToolsDefinition*                  m_pGraphDefinition = nullptr;
        TVector<TypeSystem::TypeInfo const*>            m_registeredNodeTypes;
        CategoryTree<TypeSystem::TypeInfo const*>       m_categorizedNodeTypes;
        float                                           m_primaryGraphViewHeight = 300;

        FlowGraphView                                   m_primaryFlowGraphView;
        StateMachineGraphView                           m_primaryStateMachineGraphView;
        FlowGraphView                                   m_secondaryFlowGraphView;

        GraphEditor::BaseGraphView*                     m_pPrimaryGraphView = &m_primaryFlowGraphView;
        GraphEditor::BaseGraphView*                     m_pFocusedGraphView = nullptr;
        UUID                                            m_primaryViewGraphID;
    };
}