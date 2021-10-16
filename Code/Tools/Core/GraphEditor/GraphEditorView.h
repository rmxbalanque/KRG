#pragma once
#include "GraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class KRG_TOOLS_CORE_API BaseGraphView
    {
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

        virtual BaseGraph* GetViewedGraph() = 0;
        inline BaseGraph const* GetViewedGraph() const { return const_cast<BaseGraphView*>( this )->GetViewedGraph(); }

        // Visual
        //-------------------------------------------------------------------------

        bool HasFocus() const { return m_hasFocus; }

        virtual void Draw( float childHeightOverride = 0.0f, void* pUserContext = nullptr ) = 0;

        // Selection
        //-------------------------------------------------------------------------

        // This returns whether any selection changes occurred this update, will be cleared on each call to draw
        inline bool HasSelectionChanged() const { return m_selectionChanged; }

        inline bool HasSelectedNodes() const { return !m_selectedNodes.empty(); }
        inline bool IsNodeSelected( BaseNode const* pNode ) const { return eastl::find( m_selectedNodes.begin(), m_selectedNodes.end(), pNode ) != m_selectedNodes.end(); }
        inline TVector<BaseNode*> const& GetSelectedNodes() const { return m_selectedNodes; }
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
        virtual void DrawExtraInformation( DrawingContext const& ctx ) {}
    
        // Dragging
        //-------------------------------------------------------------------------

        virtual DragMode GetDragMode() const = 0;

        inline bool IsNotDragging() const { return GetDragMode() == DragMode::None; }
        inline bool IsDraggingView() const { return GetDragMode() == DragMode::View; }
        inline bool IsDraggingSelection() const { return GetDragMode() == DragMode::Selection; }
        inline bool IsDraggingNode() const { return GetDragMode() == DragMode::Node; }
        inline bool IsDraggingConnection() const { return GetDragMode() == DragMode::Connection; }

        virtual void StartDraggingView( DrawingContext const& ctx ) = 0;
        virtual void OnDragView( DrawingContext const& ctx ) = 0;
        virtual void StopDraggingView( DrawingContext const& ctx ) = 0;

        virtual void StartDraggingSelection( DrawingContext const& ctx ) = 0;
        virtual void OnDragSelection( DrawingContext const& ctx ) = 0;
        virtual void StopDraggingSelection( DrawingContext const& ctx ) = 0;

        virtual void StartDraggingNode( DrawingContext const& ctx ) = 0;
        virtual void OnDragNode( DrawingContext const& ctx ) = 0;
        virtual void StopDraggingNode( DrawingContext const& ctx ) = 0;

        virtual void StartDraggingConnection( DrawingContext const& ctx ) = 0;
        virtual void OnDragConnection( DrawingContext const& ctx ) = 0;
        virtual void StopDraggingConnection( DrawingContext const& ctx ) = 0;

        // Selection
        //-------------------------------------------------------------------------

        void UpdateSelection( BaseNode* pNewSelectedNode );
        void UpdateSelection( TVector<BaseNode*>&& newSelection );
        void AddToSelection( BaseNode* pNodeToAdd );
        void RemoveFromSelection( BaseNode* pNodeToRemove );

        // User implementable custom selection change handler
        virtual void OnSelectionChanged( TVector<BaseNode*> const& oldSelection, TVector<BaseNode*> const& newSelection ) {}

        // Events
        //-------------------------------------------------------------------------

        virtual void OnGraphDoubleClick( BaseGraph* pGraph ) {};
        virtual void OnNodeDoubleClick( BaseNode* pNode ) {};

    private:

        KRG_FORCE_INLINE void OnSelectionChangedInternal( TVector<BaseNode*> const& oldSelection, TVector<BaseNode*> const& newSelection )
        {
            m_selectionChanged = true;
            OnSelectionChanged( oldSelection, newSelection );
        }

    protected:

        ImVec2                          m_viewOffset = ImVec2( 0, 0 );
        TVector<BaseNode*>              m_selectedNodes;
        bool                            m_hasFocus = false;
        bool                            m_selectionChanged = false;
    };
}