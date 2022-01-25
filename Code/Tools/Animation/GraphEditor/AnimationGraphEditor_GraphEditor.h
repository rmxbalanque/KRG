#pragma once
#include "Tools/Core/VisualGraph/VisualGraph_View.h"
#include "Tools/Core/Helpers/CategoryTree.h"

//-------------------------------------------------------------------------

struct ImNodesEditorContext;
namespace KRG { class UpdateContext; }
namespace KRG::Resource { class ResourceDatabase; }
namespace KRG::Animation
{
    class FlowGraph;
    class EditorGraphDefinition;
    struct DebugContext;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class GraphEditor
    {
        enum ViewID
        {
            None = -1,
            PrimaryFlow,
            PrimarySM,
            Secondary
        };

        //-------------------------------------------------------------------------

        class GraphView final : public VisualGraph::GraphView
        {
        public:

            GraphView( GraphEditor& graphEditor ) : m_graphEditor( graphEditor ) {}

        private:

            virtual void DrawContextMenuForGraph() override;
            virtual void DrawContextMenuForNode() override;
            virtual void OnGraphDoubleClick( VisualGraph::BaseGraph* pGraph ) override;
            virtual void OnNodeDoubleClick( VisualGraph::BaseNode* pNode ) override;
            virtual void OnSelectionChanged( TVector<VisualGraph::BaseNode*> const& oldSelection, TVector<VisualGraph::BaseNode*> const& newSelection ) { m_selectionChanged = true; }
            virtual void DrawExtraInformation( VisualGraph::DrawContext const& ctx ) override;
            virtual void HandleDragAndDrop( ImVec2 const& mouseCanvasPos ) override;

            // Draws the node type category - returns true if it has valid elements, false if empty
            bool DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category );

        public:

            GraphEditor&            m_graphEditor;
            bool                    m_selectionChanged = false;
            bool                    m_wasFocused = false;
        };

    public:

        GraphEditor( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDB, EditorGraphDefinition* pGraphDefinition );

        // Graph information
        EditorGraphDefinition* GetGraphDefinition() { return m_pGraphDefinition; }
        EditorGraphDefinition const* GetGraphDefinition() const { return m_pGraphDefinition; }
        inline Category<TypeSystem::TypeInfo const*> const& GetCategorizedNodeTypes() const { return m_categorizedNodeTypes.GetRootCategory(); }

        // Update
        void UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName );
        void OnUndoRedo();

        // Navigation
        void NavigateTo( VisualGraph::BaseNode* pNode );
        void NavigateTo( VisualGraph::BaseGraph* pGraph );

        // Selection
        TVector<VisualGraph::SelectedNode> const& GetSelectedNodes() const;
        void ClearSelection();

    private:

        void UpdateSecondaryViewState();
        void HandleFocusAndSelectionChanges();

    private:

        TypeSystem::TypeRegistry const&                 m_typeRegistry;
        Resource::ResourceDatabase const&               m_resourceDB;
        EditorGraphDefinition*                          m_pGraphDefinition = nullptr;
        TVector<TypeSystem::TypeInfo const*>            m_registeredNodeTypes;
        CategoryTree<TypeSystem::TypeInfo const*>       m_categorizedNodeTypes;
        float                                           m_primaryGraphViewHeight = 300;

        GraphView                                       m_primaryGraphView;
        GraphView                                       m_secondaryGraphView;

        VisualGraph::GraphView*                         m_pFocusedGraphView = nullptr;
        UUID                                            m_primaryViewGraphID;
    };
}