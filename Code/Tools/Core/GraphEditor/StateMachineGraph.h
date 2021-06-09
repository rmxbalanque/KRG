#pragma once

#include "GraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor::SM
{
    struct State
    {

    };

    struct Transition
    {

    };

    struct StateMachineGraph
    {

    };

    //-------------------------------------------------------------------------

    struct Pin
    {
        enum Type
        {
            Input,
            Output
        };

        Type        m_type;
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_CORE_API Node
    {
        inline ImRect GetCanvasRect() const
        {
            ImVec2 const rectMin = m_position - GetNodePadding();
            ImVec2 const rectMax = m_position + m_size + GetNodePadding();
            return ImRect( rectMin, rectMax );
        }

        void Draw( DrawingContext const& ctx );

    protected:

        virtual void DrawTitle( DrawingContext const& ctx );
        virtual void DrawPins( DrawingContext const& ctx );
        virtual void DrawControls( DrawingContext const& ctx );
        virtual void DrawBackground( DrawingContext const& ctx );

        virtual char const* GetTitle() const { return "Node"; }
        virtual ImVec2 GetNodePadding() const { return ImVec2( 4, 4 ); }

        // Called whenever we have mouse input and we are hovered over this node
        virtual void HandleInput();

    public:

        ImVec2              m_position = ImVec2( 0, 0 );
        ImVec2              m_size = ImVec2( 0, 0 );

        Pin*                m_pHoveredPin;
        bool                m_isHovered = false;
    };

    //-------------------------------------------------------------------------

    struct Connection
    {
        Node* m_pStartNode = nullptr;
        int32 m_startPinIdx = InvalidIndex;
        Node* m_pEndNode = nullptr;
        int32 m_endPinIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    struct Graph
    {
        virtual char const* GetTitle() const { return "Unknown"; }

        TVector<Node*>              m_nodes;
        TVector<Connection*>        m_connections;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API Canvas
    {
        enum class DragMode
        {
            None,
            View,
            Node,
            Connection,
        };

    public:

        void Draw( Graph* pGraph );

        // View
        //-------------------------------------------------------------------------

        inline void ResetView( ImVec2 const& viewStartPoint = ImVec2( 0, 0 ) )
        {
            m_viewOffset = viewStartPoint;
            m_dragMode = DragMode::None;
        }

    private:

        ImVec2      m_viewOffset = ImVec2( 0, 0 );

        //-------------------------------------------------------------------------

        Node*       m_pHoveredNode = nullptr;
        Pin*        m_pHoveredPin = nullptr;

        //-------------------------------------------------------------------------

        DragMode    m_dragMode = DragMode::None;
        ImVec2      m_dragStartValue = ImVec2( 0, 0 );
        Node*       m_pDraggedNode = nullptr;
    };
}