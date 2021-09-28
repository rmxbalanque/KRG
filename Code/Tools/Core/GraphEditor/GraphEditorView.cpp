#include "GraphEditorView.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    bool BaseGraphView::BeginDrawCanvas( float childHeightOverride )
    {
        m_selectionChanged = false;

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, VisualSettings::s_gridBackgroundColor );
        bool const childVisible = ImGui::BeginChild( "GraphCanvas", ImVec2( 0.f, childHeightOverride ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse );
        if ( childVisible )
        {
            auto pWindow = ImGui::GetCurrentWindow();
            auto pDrawList = ImGui::GetWindowDrawList();

            m_hasFocus = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows );
            pDrawList->ChannelsSplit( 4 );

            // Background
            //-------------------------------------------------------------------------

            pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::Background );

            ImRect const windowRect = pWindow->Rect();
            ImVec2 const windowTL = windowRect.GetTL();
            float const canvasWidth = windowRect.GetWidth();
            float const canvasHeight = windowRect.GetHeight();

            // Draw Grid
            for ( float x = Math::FModF( 0, VisualSettings::s_gridSpacing ); x < canvasWidth; x += VisualSettings::s_gridSpacing )
            {
                pDrawList->AddLine( windowTL + ImVec2( x, 0.0f ), windowTL + ImVec2( x, canvasHeight ), VisualSettings::s_gridLineColor );
            }

            for ( float y = Math::FModF( 0, VisualSettings::s_gridSpacing ); y < canvasHeight; y += VisualSettings::s_gridSpacing )
            {
                pDrawList->AddLine( windowTL + ImVec2( 0.0f, y ), windowTL + ImVec2( canvasWidth, y ), VisualSettings::s_gridLineColor );
            }

            // Draw title
            auto pViewedGraph = GetViewedGraph();
            if ( pViewedGraph != nullptr )
            {
                ImGuiX::ScopedFont font( ImGuiX::Font::ExtraLarge );
                pDrawList->AddText( windowRect.Min + VisualSettings::s_graphTitleMargin, VisualSettings::s_graphTitleColor, pViewedGraph->GetTitle() );
            }
        }

        return childVisible;
    }

    void BaseGraphView::EndDrawCanvas()
    {
        ImGui::GetWindowDrawList()->ChannelsMerge();

        //-------------------------------------------------------------------------

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void BaseGraphView::ClearSelection()
    {
        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void BaseGraphView::UpdateSelection( BaseNode* pNewSelectedNode )
    {
        KRG_ASSERT( pNewSelectedNode != nullptr );

        // Avoid calling the notification if the selection hasn't changed
        if ( m_selectedNodes.size() == 1 && m_selectedNodes[0] == pNewSelectedNode )
        {
            return;
        }

        //-------------------------------------------------------------------------

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNewSelectedNode );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void BaseGraphView::UpdateSelection( TVector<BaseNode*>&& newSelection )
    {
        for ( auto pNode : newSelection )
        {
            KRG_ASSERT( pNode != nullptr );
        }

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.swap( newSelection );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void BaseGraphView::AddToSelection( BaseNode* pNodeToAdd )
    {
        KRG_ASSERT( pNodeToAdd != nullptr );
        KRG_ASSERT( !IsNodeSelected( pNodeToAdd ) );

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNodeToAdd );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void BaseGraphView::RemoveFromSelection( BaseNode* pNodeToRemove )
    {
        KRG_ASSERT( pNodeToRemove != nullptr );
        KRG_ASSERT( IsNodeSelected( pNodeToRemove ) );

        TVector<BaseNode*> oldSelection = m_selectedNodes;
        m_selectedNodes.erase_first( pNodeToRemove );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void BaseGraphView::ResetInternalState()
    {
        m_viewOffset = ImVec2( 0, 0 );
        m_hasFocus = false;
        m_selectionChanged = false;
        ClearSelection();
    }
}