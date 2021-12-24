#include "TreeListView.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ImVec4 TreeListViewItem::GetDisplayColor( bool isActive ) const
    {
        return isActive ? Colors::GreenYellow.ToFloat4() : (ImVec4) ImGuiX::Style::s_textColor;
    }

    void TreeListViewItem::DestroyChildren()
    {
        for ( auto& pChild : m_children )
        {
            pChild->DestroyChildren();
            KRG::Delete( pChild );
        }

        m_children.clear();
    }

    TreeListViewItem* TreeListViewItem::FindChild( TFunction<bool( TreeListViewItem const* )> const& searchPredicate )
    {
        TreeListViewItem* pFoundItem = nullptr;
        auto itemIter = eastl::find_if( m_children.begin(), m_children.end(), searchPredicate );
        if ( itemIter != m_children.end() )
        {
            pFoundItem = *itemIter;
        }
        return pFoundItem;
    }

    TreeListViewItem* TreeListViewItem::SearchChildren( TFunction<bool( TreeListViewItem const* )> const& searchPredicate )
    {
        TreeListViewItem* pFoundItem = nullptr;

        // Search immediate children
        //-------------------------------------------------------------------------

        auto itemIter = eastl::find_if( m_children.begin(), m_children.end(), searchPredicate );
        if ( itemIter != m_children.end() )
        {
            pFoundItem = *itemIter;
            return pFoundItem;
        }

        // Recursively search each child
        //-------------------------------------------------------------------------

        for ( auto pChild : m_children )
        {
            pFoundItem = pChild->SearchChildren( searchPredicate );
            if ( pFoundItem != nullptr )
            {
                return pFoundItem;
            }
        }

        return pFoundItem;
    }

    void TreeListViewItem::UpdateVisibility( TFunction<bool( TreeListViewItem const* pItem )> const& isVisibleFunction, bool showParentItemsWithNoVisibleChildren )
    {
        if ( HasChildren() )
        {
            // Always update child visibility before our own - this allows clients to affect our visibility based on our childrens' visibility
            bool hasVisibleChild = false;
            for ( auto& pChild : m_children )
            {
                pChild->UpdateVisibility( isVisibleFunction );
                if ( pChild->IsVisible() )
                {
                    hasVisibleChild = true;
                }
            }

            // If we have a visible child, we are always visible
            if ( hasVisibleChild )
            {
                m_isVisible = true;
            }
            else // Optional: run visibility check if requested
            {
                m_isVisible = showParentItemsWithNoVisibleChildren ? isVisibleFunction( this ) : false;
            }
        }
        else // Leaf nodes always run visibility check
        {
            m_isVisible = isVisibleFunction( this );
        }
    }

    //-------------------------------------------------------------------------

    TreeListView::~TreeListView()
    {
        if ( m_pRoot != nullptr )
        {
            m_pRoot->DestroyChildren();
            KRG::Delete( m_pRoot );
        }
    }

    //-------------------------------------------------------------------------

    void TreeListView::TryAddItemToVisualTree( TreeListViewItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );

        //-------------------------------------------------------------------------

        if ( !pItem->IsVisible() )
        {
            return;
        }

        m_visualTree.emplace_back( pItem );

        //-------------------------------------------------------------------------

        if ( pItem->IsExpanded() )
        {
            // Always add branch items first
            for ( auto& pChildItem : pItem->m_children )
            {
                if ( pChildItem->HasChildren() )
                {
                    TryAddItemToVisualTree( pChildItem );
                }
            }

            // Add leaf items last
            for ( auto& pChildItem : pItem->m_children )
            {
                if ( !pChildItem->HasChildren() )
                {
                    TryAddItemToVisualTree( pChildItem );
                }
            }
        }
    }

    void TreeListView::UpdateVisualTree()
    {
        KRG_ASSERT( m_visualTreeState != VisualTreeState::UpToDate );
        KRG_ASSERT( m_pRoot != nullptr && m_pRoot->GetUniqueID() != 0 );

        //-------------------------------------------------------------------------

        m_visualTree.clear();

        // Always add branch items first
        for ( auto& pChildItem : m_pRoot->m_children )
        {
            if ( pChildItem->HasChildren() )
            {
                TryAddItemToVisualTree( pChildItem );
            }
        }

        // Add leaf items last
        for ( auto& pChildItem : m_pRoot->m_children )
        {
            if ( !pChildItem->HasChildren() )
            {
                TryAddItemToVisualTree( pChildItem );
            }
        }

        m_estimatedRowHeight = -1.0f;
        m_estimatedTreeHeight = -1.0f;

        //-------------------------------------------------------------------------

        // Reset view
        if ( m_visualTreeState == VisualTreeState::NeedsRebuildAndViewReset )
        {
            m_firstVisibleRowItemIdx = 0;
        }

        m_visualTreeState = VisualTreeState::UpToDate;
    }

    void TreeListView::OnItemDoubleClickedInternal( TreeListViewItem* pItem )
    {
        // Double click
        //-------------------------------------------------------------------------

        m_onItemDoubleClicked.Execute( pItem );

        // Activation
        //-------------------------------------------------------------------------

        if ( !pItem->IsActivatable() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pActiveItem == pItem )
        {
            m_pActiveItem = nullptr;
        }
        else
        {
            m_pActiveItem = pItem;
        }

        //-------------------------------------------------------------------------

        m_onActiveItemChanged.Execute( m_pActiveItem );
    }

    //-------------------------------------------------------------------------

    void TreeListView::DrawVisualItem( TreeListViewItem* pItem )
    {
        ImGui::PushID( pItem );
        ImGui::TableNextRow();

        // Draw label column
        //-------------------------------------------------------------------------

        ImGui::TableSetColumnIndex( 0 );

        // Set tree indent level
        float const indentLevel = pItem->m_hierarchyLevel * ImGui::GetStyle().IndentSpacing;
        bool const requiresIndent = indentLevel > 0;
        if ( requiresIndent )
        {
            ImGui::Indent( indentLevel );
        }

        // Set node flags
        uint32 treeNodeflags = ImGuiTreeNodeFlags_SpanFullWidth;
        if ( m_pSelectedItem == pItem )
        {
            treeNodeflags |= ImGuiTreeNodeFlags_Selected;
        }

        // Leaf nodes
        if ( pItem->m_children.empty() )
        {
            treeNodeflags |= ImGuiTreeNodeFlags_Leaf;
        }
        else // Branch nodes
        {
            ImGui::SetNextItemOpen( pItem->IsExpanded() );
        }

        // Draw label
        bool newExpansionState = false;
        {
            bool const isActiveItem = pItem == m_pActiveItem;
            ImGuiX::ScopedFont font( isActiveItem ? ImGuiX::Font::SmallBold : ImGuiX::Font::Small, pItem->GetDisplayColor( isActiveItem ) );
            newExpansionState = ImGui::TreeNodeEx( pItem->GetDisplayName(), treeNodeflags );
        }

        // Handle expansion
        if ( newExpansionState )
        {
            ImGui::TreePop();
        }

        if ( pItem->HasChildren() && pItem->IsExpanded() != newExpansionState )
        {
            pItem->SetExpanded( newExpansionState );
            m_visualTreeState = VisualTreeState::NeedsRebuild;
        }

        // Handle selection
        if ( ImGui::IsItemClicked( ImGuiMouseButton_Left ) || ImGui::IsItemClicked( ImGuiMouseButton_Right ) || ( ImGui::IsItemFocused() && ImGui::IsKeyReleased( ImGui::GetKeyIndex( ImGuiKey_Enter ) ) ) )
        {
            if ( m_pSelectedItem != pItem )
            {
                m_pSelectedItem = pItem;
                m_onSelectionChanged.Execute();
            }
        }

        // Context Menu
        if ( pItem->HasContextMenu() )
        {
            if ( ImGui::BeginPopupContextItem( "ItemContextMenu" ) )
            {
                m_pSelectedItem = pItem;
                m_onSelectionChanged.Execute();

                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().ItemSpacing.x ) ); // Undo the table item spacing changes
                DrawItemContextMenu( pItem );
                ImGui::PopStyleVar();

                ImGui::EndPopup();
            }
        }

        // Handle double click
        if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( 0 ) )
        {
            OnItemDoubleClickedInternal( pItem );
        }

        // Pop indent level if set
        if ( requiresIndent )
        {
            ImGui::Unindent( indentLevel );
        }

        // Draw extra columns
        //-------------------------------------------------------------------------

        for ( uint32 i = 0u; i < GetNumExtraColumns(); i++ )
        {
            ImGui::TableSetColumnIndex( i + 1 );
            DrawItemExtraColumns( pItem, i );
        }

        //-------------------------------------------------------------------------

        ImGui::PopID();
    }

    void TreeListView::Draw()
    {
        if ( m_pRoot == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_visualTreeState != VisualTreeState::UpToDate )
        {
            UpdateVisualTree();
        }

        if ( m_visualTree.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( ImGui::GetStyle().ItemSpacing.x, 0 ) ); // Ensure table border and scrollbar align
        ImGui::BeginChild( "TreeViewChild", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );
        {
            float const totalVerticalSpaceAvailable = ImGui::GetContentRegionAvail().y;
            float const maxVerticalScrollPosition = ImGui::GetScrollMaxY();

            //-------------------------------------------------------------------------

            // Calculate the required height in pixels for all rows based on the height of the first row. 
            // We only render a single row here (so we dont bust the imgui index buffer limits for a single tree) - At low frame rates this will flicker
            if ( m_estimatedTreeHeight < 0 )
            {
                ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
                ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Style::s_itemColorMedium.Value );
                constexpr static ImGuiTableFlags const flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
                if ( ImGui::BeginTable( "TreeViewTable", GetNumExtraColumns() + 1, flags, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, 0)) )
                {
                    ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_NoHide );
                    SetupExtraColumnHeaders();

                    ImVec2 const prevCursorPos = ImGui::GetCursorPos();
                    DrawVisualItem( m_visualTree[0] );
                    ImGui::TableNextRow();
                    ImVec2 const cursorPos = ImGui::GetCursorPos();
                    m_estimatedRowHeight = cursorPos.y - prevCursorPos.y;

                    ImGui::EndTable();
                }
                ImGui::PopStyleColor( 2 );

                // Draw a dummy to fake the real size so as to not completely reset scrollbar position
                //-------------------------------------------------------------------------

                m_estimatedTreeHeight = float( m_visualTree.size() ) * m_estimatedRowHeight;
                ImGui::Dummy( ImVec2( -1, m_estimatedTreeHeight - ImGui::GetCursorPos().y ) );

                // Update scrollbar position
                //-------------------------------------------------------------------------

                m_firstVisibleRowItemIdx = Math::Clamp( m_firstVisibleRowItemIdx, 0, int32( m_visualTree.size() - 1 ) );
                m_maintainVisibleRowIdx = true;
            }
            else // Draw clipped table
            {
                float const numRowIndices = float( m_visualTree.size() ) - 1;
                float currentVerticalScrollPosition = ImGui::GetScrollY();

                // If we want to maintain the current visible set of data, update the scroll bar position to keep the same first visible item
                if ( m_maintainVisibleRowIdx )
                {
                    currentVerticalScrollPosition = ( m_firstVisibleRowItemIdx / numRowIndices * maxVerticalScrollPosition );
                    ImGui::SetScrollY( currentVerticalScrollPosition );
                    m_maintainVisibleRowIdx = false;
                }

                // Update visible item based on scrollbar position
                m_firstVisibleRowItemIdx = (int32) Math::Round( ( currentVerticalScrollPosition / maxVerticalScrollPosition ) * numRowIndices );
                m_firstVisibleRowItemIdx = Math::Clamp( m_firstVisibleRowItemIdx, 0, (int32) m_visualTree.size() - 1 );

                // Calculate draw range
                bool shouldDrawDummyRow = false;
                int32 const maxNumDrawableRows = (int32) Math::Floor( totalVerticalSpaceAvailable / m_estimatedRowHeight );
                int32 const itemsToDrawStartIdx = m_firstVisibleRowItemIdx;
                int32 itemsToDrawEndIdx = Math::Min( itemsToDrawStartIdx + maxNumDrawableRows, (int32) m_visualTree.size() - 1 );

                // Draw initial dummy to adjust scrollbar position
                ImGui::Dummy( ImVec2( -1, currentVerticalScrollPosition ) );

                // Draw table rows
                ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
                ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Style::s_itemColorSemiLight.Value );
                constexpr static ImGuiTableFlags const flagss = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Resizable;
                if ( ImGui::BeginTable( "Tree View Browser", GetNumExtraColumns() + 1, flagss, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, 0)) )
                {
                    ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthStretch );
                    SetupExtraColumnHeaders();

                    for ( int32 i = itemsToDrawStartIdx; i <= itemsToDrawEndIdx; i++ )
                    {
                        DrawVisualItem( m_visualTree[i] );
                    }

                    ImGui::EndTable();
                }
                ImGui::PopStyleColor( 2 );

                // Draw final dummy to maintain scrollbar position
                ImGui::Dummy( ImVec2( -1, m_estimatedTreeHeight - ImGui::GetCursorPos().y ) );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopID();

        //-------------------------------------------------------------------------

        DrawAdditionalUI();
    }
}