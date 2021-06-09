#pragma once
#include "FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class KRG_TOOLS_CORE_API FlowGraphView
    {
        struct DragState
        {
            void Reset()
            {
                m_mode = DragMode::None;
                m_startValue = m_lastFrameDragDelta = ImVec2( 0, 0 );
                m_pDraggedNode = nullptr;
                m_pDraggedPin = nullptr;
            }

            DragMode                m_mode = DragMode::None;
            ImVec2                  m_startValue;
            ImVec2                  m_lastFrameDragDelta;
            Flow::Node*             m_pDraggedNode = nullptr;
            Flow::Pin*              m_pDraggedPin = nullptr;
        };

        struct ContextMenuState
        {
            inline bool IsNodeContextMenu() const { return m_pNode != nullptr; }
            void Reset() { m_pNode = nullptr; m_pPin = nullptr; }

            ImVec2                  m_mouseCanvasPos;
            Flow::Node*             m_pNode = nullptr;
            Flow::Pin*              m_pPin = nullptr;
            bool                    m_menuOpened = false;
        };

        constexpr static float const s_pinRadius = 5.0f;
        constexpr static float const s_pinSelectionExtraRadius = 5.0f;
        constexpr static float const s_connectionSelectionExtraRadius = 5.0f;

    public:

        FlowGraphView() = default;

        void SetGraphToView( FlowGraph* pGraph );
        FlowGraph const* GetViewedGraph() const { return m_pGraph; }

        bool HasFocus() const { return m_hasFocus; }
        void Draw( float childHeightOverride = 0.0f );

        // Selection
        //-------------------------------------------------------------------------

        inline bool HasSelectedNodes() const { return !m_selectedNodes.empty(); }
        inline bool IsNodeSelected( BaseNode const* pNode ) const { return eastl::find( m_selectedNodes.begin(), m_selectedNodes.end(), pNode ) != m_selectedNodes.end(); }
        inline TVector<BaseNode*> const& GetSelectedNodes() const { return m_selectedNodes; }
        void ClearSelection();

    protected:

        // Node
        //-------------------------------------------------------------------------

        inline ImRect GetNodeCanvasRect( Flow::Node* pNode ) const
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

        void DrawNode( DrawingContext const& ctx, Flow::Node* pNode );
        void DrawNodeTitle( DrawingContext const& ctx, Flow::Node* pNode );
        void DrawNodePins( DrawingContext const& ctx, Flow::Node* pNode );
        void DrawNodeBackground( DrawingContext const& ctx, Flow::Node* pNode );

        // Graph
        //-------------------------------------------------------------------------

        inline void ResetGraphView( ImVec2 const& viewStartPoint = ImVec2( 0, 0 ) )
        {
            m_viewOffset = viewStartPoint;
            m_dragState.Reset();
        }

        void HandleClicks( DrawingContext const& ctx );
        void HandleContextMenu( DrawingContext const& ctx );
        void HandleDragging( DrawingContext const& ctx );

        // Dragging
        //-------------------------------------------------------------------------

        inline bool IsNotDragging() const { return m_dragState.m_mode == DragMode::None; }

        inline bool IsDraggingView() const { return m_dragState.m_mode == DragMode::View; }
        void StartDraggingView();
        void OnDragView( DrawingContext const& ctx );
        void StopDraggingView( DrawingContext const& ctx );

        inline bool IsDraggingSelection() const { return m_dragState.m_mode == DragMode::Selection; }
        void StartDraggingSelection();
        void OnDragSelection( DrawingContext const& ctx );
        void StopDraggingSelection( DrawingContext const& ctx );

        inline bool IsDraggingNode() const { return m_dragState.m_mode == DragMode::Node; }
        void StartDraggingNode();
        void OnDragNode( DrawingContext const& ctx );
        void StopDraggingNode( DrawingContext const& ctx );

        inline bool IsDraggingConnection() const { return m_dragState.m_mode == DragMode::Connection; }
        void StartDraggingConnection();
        void OnDragConnection( DrawingContext const& ctx );
        void StopDraggingConnection( DrawingContext const& ctx );

        // Context Menus
        //-------------------------------------------------------------------------

        void DrawContextMenu( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin );
        inline bool IsContextMenuOpen() const { return m_contextMenuState.m_menuOpened; }

        virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos );
        virtual void DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, Flow::Node* pNode );
        virtual void DrawContextMenuForPin( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin );

        // Selection
        //-------------------------------------------------------------------------

        void UpdateSelection( BaseNode* pNewSelectedNode );
        void UpdateSelection( TVector<BaseNode*>&& newSelection );
        void AddToSelection( BaseNode* pNodeToAdd );
        void RemoveFromSelection( BaseNode* pNodeToRemove );

        virtual void OnSelectionChanged( TVector<BaseNode*> const& oldSelection, TVector<BaseNode*> const& newSelection ) {}

    protected:

        FlowGraph*                      m_pGraph = nullptr;
        ImVec2                          m_viewOffset = ImVec2( 0, 0 );
        Flow::Node*                     m_pHoveredNode = nullptr;
        Flow::Pin*                      m_pHoveredPin = nullptr;
        UUID                            m_hoveredConnectionID;
        ContextMenuState                m_contextMenuState;
        TVector<BaseNode*>              m_selectedNodes;
        DragState                       m_dragState;
        bool                            m_hasFocus = false;
    };
}