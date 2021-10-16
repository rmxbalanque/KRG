#pragma once
#include "FlowGraph.h"
#include "../GraphEditorView.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class KRG_TOOLS_CORE_API FlowGraphView : public BaseGraphView
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

    public:

        FlowGraphView() = default;

        void SetGraphToView( FlowGraph* pGraph );

        virtual BaseGraph* GetViewedGraph() override final { return m_pGraph; }
        virtual DragMode GetDragMode() const final { return m_dragState.m_mode; }
        virtual void Draw( float childHeightOverride = 0.0f, void* pUserContext = nullptr ) override final;

    protected:

        virtual void ResetInternalState() override;

        // Visuals
        //-------------------------------------------------------------------------

        void DrawNode( DrawingContext const& ctx, Flow::Node* pNode );
        void DrawNodeTitle( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );
        void DrawNodePins( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );
        void DrawNodeBackground( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );

        // Events
        //-------------------------------------------------------------------------

        void HandleClicks( DrawingContext const& ctx );
        void HandleContextMenu( DrawingContext const& ctx );
        void HandleDragging( DrawingContext const& ctx );

        // Dragging
        //-------------------------------------------------------------------------

        virtual void StartDraggingView( DrawingContext const& ctx ) override;
        virtual void OnDragView( DrawingContext const& ctx ) override;
        virtual void StopDraggingView( DrawingContext const& ctx ) override;

        virtual void StartDraggingSelection( DrawingContext const& ctx ) override;
        virtual void OnDragSelection( DrawingContext const& ctx ) override;
        virtual void StopDraggingSelection( DrawingContext const& ctx ) override;

        virtual void StartDraggingNode( DrawingContext const& ctx ) override;
        virtual void OnDragNode( DrawingContext const& ctx ) override;
        virtual void StopDraggingNode( DrawingContext const& ctx ) override;

        virtual void StartDraggingConnection( DrawingContext const& ctx ) override;
        virtual void OnDragConnection( DrawingContext const& ctx ) override;
        virtual void StopDraggingConnection( DrawingContext const& ctx ) override;

        // Context Menus
        //-------------------------------------------------------------------------

        void DrawContextMenu( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin );
        inline bool IsContextMenuOpen() const { return m_contextMenuState.m_menuOpened; }

        // User derivable to provide extra option for the context menu
        virtual void DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos );

        // User derivable to provide extra options for the context menu
        virtual void DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin ) {}

    protected:

        FlowGraph*                      m_pGraph = nullptr;
        Flow::Node*                     m_pHoveredNode = nullptr;
        Flow::Pin*                      m_pHoveredPin = nullptr;
        UUID                            m_hoveredConnectionID;
        ContextMenuState                m_contextMenuState;
        DragState                       m_dragState;
    };
}