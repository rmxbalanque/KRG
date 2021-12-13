#pragma once
#include "GraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class KRG_TOOLS_CORE_API BaseGraphView
    {

    public:

        // Helper to unsure we can maintain selection after a undo/redo
        struct SelectedNode
        {
            SelectedNode( BaseNode* pNode ) : m_nodeID( pNode->GetID() ), m_pNode( pNode ) {}
            bool operator==( SelectedNode const& rhs ) const { return m_nodeID == rhs.m_nodeID; }
            bool operator==( BaseNode const* pNode ) const { return m_nodeID == pNode->GetID(); }

            UUID            m_nodeID;
            BaseNode*       m_pNode = nullptr;
        };

    protected:

        enum class DragMode
        {
            None,
            View,
            Selection,
            Node,
            Connection,
        };

        //-------------------------------------------------------------------------

        enum class DrawChannel
        {
            Background = 0,
            NodeBackground = 1,
            NodeForeground = 2,
            Connections = 3
        };

    public:

        bool HasFocus() const { return m_hasFocus; }

        virtual BaseGraph* GetViewedGraph() = 0;
        inline BaseGraph const* GetViewedGraph() const { return const_cast<BaseGraphView*>( this )->GetViewedGraph(); }

        virtual void UpdateAndDraw( float childHeightOverride = 0.0f, void* pUserContext = nullptr ) = 0;

        // Selection
        //-------------------------------------------------------------------------

        // This returns whether any selection changes occurred this update, will be cleared on each call to draw
        inline bool HasSelectionChanged() const { return m_selectionChanged; }

        inline void SelectNode( BaseNode const* pNode );
        inline bool HasSelectedNodes() const { return !m_selectedNodes.empty(); }
        inline bool IsNodeSelected( BaseNode const* pNode ) const { return eastl::find( m_selectedNodes.begin(), m_selectedNodes.end(), pNode ) != m_selectedNodes.end(); }
        inline TVector<SelectedNode> const& GetSelectedNodes() const { return m_selectedNodes; }
        void ClearSelection();

    protected:

        virtual void ResetInternalState();

        // Node
        //-------------------------------------------------------------------------

        inline ImRect GetNodeCanvasRect( BaseNode* pNode ) const
        {
            ImVec2 const nodeMargin = pNode->GetNodeMargin();
            ImVec2 const rectMin = ImVec2( pNode->GetCanvasPosition() ) - nodeMargin;
            ImVec2 const rectMax = ImVec2( pNode->GetCanvasPosition() ) + pNode->GetSize() + nodeMargin;
            return ImRect( rectMin, rectMax );
        }

        inline ImRect GetNodeWindowRect( BaseNode* pNode ) const
        {
            ImVec2 const nodeMargin = pNode->GetNodeMargin();
            ImVec2 const rectMin = ImVec2( pNode->GetCanvasPosition() ) - nodeMargin - m_viewOffset;
            ImVec2 const rectMax = ImVec2( pNode->GetCanvasPosition() ) + pNode->GetSize() + nodeMargin - m_viewOffset;
            return ImRect( rectMin, rectMax );
        }

        // Visual
        //-------------------------------------------------------------------------

        bool BeginDrawCanvas( float childHeightOverride );
        void EndDrawCanvas();

        // User implementable function to draw any additional information needed in the graph (called after everything is drawn)
        virtual void DrawExtraInformation( DrawContext const& ctx ) {}
    
        // Dragging
        //-------------------------------------------------------------------------

        virtual DragMode GetDragMode() const = 0;

        inline bool IsNotDragging() const { return GetDragMode() == DragMode::None; }
        inline bool IsDraggingView() const { return GetDragMode() == DragMode::View; }
        inline bool IsDraggingSelection() const { return GetDragMode() == DragMode::Selection; }
        inline bool IsDraggingNode() const { return GetDragMode() == DragMode::Node; }
        inline bool IsDraggingConnection() const { return GetDragMode() == DragMode::Connection; }

        virtual void StartDraggingView( DrawContext const& ctx ) = 0;
        virtual void OnDragView( DrawContext const& ctx ) = 0;
        virtual void StopDraggingView( DrawContext const& ctx ) = 0;

        virtual void StartDraggingSelection( DrawContext const& ctx ) = 0;
        virtual void OnDragSelection( DrawContext const& ctx ) = 0;
        virtual void StopDraggingSelection( DrawContext const& ctx ) = 0;

        virtual void StartDraggingNode( DrawContext const& ctx ) = 0;
        virtual void OnDragNode( DrawContext const& ctx ) = 0;
        virtual void StopDraggingNode( DrawContext const& ctx ) = 0;

        virtual void StartDraggingConnection( DrawContext const& ctx ) = 0;
        virtual void OnDragConnection( DrawContext const& ctx ) = 0;
        virtual void StopDraggingConnection( DrawContext const& ctx ) = 0;

        // Selection
        //-------------------------------------------------------------------------

        void UpdateSelection( BaseNode* pNewSelectedNode );
        void UpdateSelection( TVector<SelectedNode>&& newSelection );
        void AddToSelection( BaseNode* pNodeToAdd );
        void RemoveFromSelection( BaseNode* pNodeToRemove );

        // User implementable custom selection change handler
        virtual void OnSelectionChanged( TVector<SelectedNode> const& oldSelection, TVector<SelectedNode> const& newSelection ) {}

        // Events
        //-------------------------------------------------------------------------

        virtual void OnGraphDoubleClick( BaseGraph* pGraph ) {};
        virtual void OnNodeDoubleClick( BaseNode* pNode ) {};

    private:

        KRG_FORCE_INLINE void OnSelectionChangedInternal( TVector<SelectedNode> const& oldSelection, TVector<SelectedNode> const& newSelection )
        {
            m_selectionChanged = true;
            OnSelectionChanged( oldSelection, newSelection );
        }

    protected:

        ImVec2                          m_viewOffset = ImVec2( 0, 0 );
        TVector<SelectedNode>           m_selectedNodes;
        bool                            m_hasFocus = false;
        bool                            m_selectionChanged = false;
    };
}