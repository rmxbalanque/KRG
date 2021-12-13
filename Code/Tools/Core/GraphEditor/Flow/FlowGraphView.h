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

        void SetGraphToView( FlowGraph* pGraph, bool tryMaintainSelection = false );

        virtual BaseGraph* GetViewedGraph() override final { return m_pGraph; }
        virtual DragMode GetDragMode() const final { return m_dragState.m_mode; }
        virtual void UpdateAndDraw( float childHeightOverride = 0.0f, void* pUserContext = nullptr ) override final;

    protected:

        virtual void ResetInternalState() override;

        // Visuals
        //-------------------------------------------------------------------------

        void DrawNode( DrawContext const& ctx, Flow::Node* pNode );
        void DrawNodeTitle( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );
        void DrawNodePins( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );
        void DrawNodeBackground( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize );

        // Events
        //-------------------------------------------------------------------------

        void HandleInput( DrawContext const& ctx );
        void HandleContextMenu( DrawContext const& ctx );

        // Dragging
        //-------------------------------------------------------------------------

        virtual void StartDraggingView( DrawContext const& ctx ) override;
        virtual void OnDragView( DrawContext const& ctx ) override;
        virtual void StopDraggingView( DrawContext const& ctx ) override;

        virtual void StartDraggingSelection( DrawContext const& ctx ) override;
        virtual void OnDragSelection( DrawContext const& ctx ) override;
        virtual void StopDraggingSelection( DrawContext const& ctx ) override;

        virtual void StartDraggingNode( DrawContext const& ctx ) override;
        virtual void OnDragNode( DrawContext const& ctx ) override;
        virtual void StopDraggingNode( DrawContext const& ctx ) override;

        virtual void StartDraggingConnection( DrawContext const& ctx ) override;
        virtual void OnDragConnection( DrawContext const& ctx ) override;
        virtual void StopDraggingConnection( DrawContext const& ctx ) override;

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