#include "TreeListView.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ImVec4 TreeListViewItem::GetDisplayColor( ItemState state ) const
    {
        ImVec4 color = (ImVec4) ImGuiX::Style::s_textColor;

        switch ( state )
        {
            case ItemState::Selected:
            {
                color = ImGuiX::Style::s_selectionAccent;
            }
            break;

            case ItemState::Active:
            {
                color = ImGuiX::Style::s_selectionAccentAlt;
            }
            break;
        }

        return color;
    }

    void TreeListViewItem::DestroyChild( uint64 uniqueItemID )
    {
        for ( int32 i = 0; i < (int32) m_children.size(); i++ )
        {
            if ( m_children[i]->GetUniqueID() == uniqueItemID )
            {
                m_children[i]->DestroyChildren();
                KRG::Delete( m_children[i] );
                m_children.erase( m_children.begin() + i );
                break;
            }
        }
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

    TreeListViewItem const* TreeListViewItem::FindChild( uint64 uniqueID ) const 
    {
        for ( auto pChild : m_children )
        {
            if ( pChild->GetUniqueID() == uniqueID )
            {
                return pChild;
            }
        }

        return nullptr;
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
            // Always update child visibility before our own - this allows clients to affect our visibility based on our children's visibility
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
        m_rootItem.DestroyChildren();
    }

    TreeListViewItem* TreeListView::FindItem( uint64 uniqueID )
    {
        if ( m_rootItem.GetUniqueID() == uniqueID )
        {
            return &m_rootItem;
        }

        auto SearchPredicate = [uniqueID] ( TreeListViewItem const* pItem )
        {
            return pItem->GetUniqueID() == uniqueID;
        };

        auto pFoundItem = m_rootItem.SearchChildren( SearchPredicate );
        return pFoundItem;
    }

    void TreeListView::DestroyItem( uint64 uniqueID )
    {
        KRG_ASSERT( m_rootItem.GetUniqueID() != uniqueID );

        // Remove from visual tree
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_visualTree.size(); i++ )
        {
            if ( m_visualTree[i].m_pItem->GetUniqueID() == uniqueID )
            {
                m_visualTree.erase( m_visualTree.begin() + i );
                break;
            }
        }

        // Remove for regular tree
        //-------------------------------------------------------------------------

        auto SearchPredicate = [uniqueID] ( TreeListViewItem const* pItem )
        {
            return pItem->FindChild( uniqueID ) != nullptr;
        };

        auto pFoundParentItem = m_rootItem.SearchChildren( SearchPredicate );
        KRG_ASSERT( pFoundParentItem != nullptr );
        pFoundParentItem->DestroyChild( uniqueID );
    }

    void TreeListView::RebuildTree( bool maintainExpansionAndSelection )
    {
        // Record current state
        //-------------------------------------------------------------------------

        uint64 activeItemID = 0;
        TVector<uint64> selectedItemIDs;
        TVector<uint64> originalExpandedItems;

        if ( maintainExpansionAndSelection )
        {
            if( m_pActiveItem != nullptr )
            {
                activeItemID = m_pActiveItem->GetUniqueID();
            }

            for ( auto pSelectedItem : m_selection )
            {
                selectedItemIDs.emplace_back( pSelectedItem->GetUniqueID() );
            }

            originalExpandedItems.reserve( GetNumItems() );

            auto RecordItemState = [&originalExpandedItems] ( TreeListViewItem const* pItem )
            {
                if ( pItem->IsExpanded() )
                {
                    originalExpandedItems.emplace_back( pItem->GetUniqueID() );
                }
            };

            ForEachItemConst( RecordItemState );
        }

        // Reset tree state
        //-------------------------------------------------------------------------

        m_pActiveItem = nullptr;
        m_selection.clear();
        m_rootItem.DestroyChildren();
        m_rootItem.SetExpanded( true );

        // Rebuild Tree
        //-------------------------------------------------------------------------

        RebuildTreeInternal();

        // Restore original state
        //-------------------------------------------------------------------------

        if ( maintainExpansionAndSelection )
        {
            auto RestoreItemState = [&originalExpandedItems] ( TreeListViewItem* pItem )
            {
                if ( VectorContains( originalExpandedItems, pItem->GetUniqueID() ) )
                {
                    pItem->SetExpanded( true );
                }
            };

            ForEachItem( RestoreItemState );

            auto FindActiveItem = [activeItemID] ( TreeListViewItem const* pItem ) { return pItem->GetUniqueID() == activeItemID; };
            m_pActiveItem = m_rootItem.SearchChildren( FindActiveItem );

            for ( auto selectedItemID : selectedItemIDs )
            {
                auto FindSelectedItem = [selectedItemID] ( TreeListViewItem const* pItem )
                {
                    return pItem->GetUniqueID() == selectedItemID; 
                };

                m_selection.emplace_back( m_rootItem.SearchChildren( FindSelectedItem ) );
            }
        }

        RefreshVisualState();
    }

    //-------------------------------------------------------------------------

    void TreeListView::TryAddItemToVisualTree( TreeListViewItem* pItem, int32 hierarchyLevel )
    {
        KRG_ASSERT( pItem != nullptr );
        KRG_ASSERT( hierarchyLevel >= 0 );

        //-------------------------------------------------------------------------

        if ( !pItem->IsVisible() )
        {
            return;
        }

        m_visualTree.emplace_back( pItem, hierarchyLevel );

        //-------------------------------------------------------------------------

        if ( pItem->IsExpanded() )
        {
            // Always add branch items first
            for ( auto& pChildItem : pItem->m_children )
            {
                if ( pChildItem->HasChildren() )
                {
                    TryAddItemToVisualTree( pChildItem, hierarchyLevel + 1 );
                }
            }

            // Add leaf items last
            for ( auto& pChildItem : pItem->m_children )
            {
                if ( !pChildItem->HasChildren() )
                {
                    TryAddItemToVisualTree( pChildItem, hierarchyLevel + 1 );
                }
            }
        }
    }

    void TreeListView::RebuildVisualTree()
    {
        KRG_ASSERT( m_visualTreeState != VisualTreeState::UpToDate );

        //-------------------------------------------------------------------------

        m_visualTree.clear();

        // Always add branch items first
        for ( auto& pChildItem : m_rootItem.m_children )
        {
            if ( pChildItem->HasChildren() )
            {
                TryAddItemToVisualTree( pChildItem, 0 );
            }
        }

        // Add leaf items last
        for ( auto& pChildItem : m_rootItem.m_children )
        {
            if ( !pChildItem->HasChildren() )
            {
                TryAddItemToVisualTree( pChildItem, 0 );
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

        if ( m_pActiveItem == pItem )
        {
            m_pActiveItem = nullptr;
        }
        else
        {
            m_pActiveItem = pItem;
        }

        m_onActiveItemChanged.Execute();
    }

    //-------------------------------------------------------------------------

    void TreeListView::SelectItem( TreeListViewItem* pItem )
    {
        m_selection.clear();
        m_selection.emplace_back( pItem );
        NotifySelectionChanged();
    }

    void TreeListView::AddToSelection( TreeListViewItem* pItem )
    {
        KRG_ASSERT( m_multiSelectionAllowed );

        if ( !VectorContains( m_selection, pItem ) )
        {
            m_selection.emplace_back( pItem );
            NotifySelectionChanged();
        }
    }

    void TreeListView::RemoveFromSelection( TreeListViewItem* pItem )
    {
        KRG_ASSERT( m_multiSelectionAllowed );

        if ( VectorContains( m_selection, pItem ) )
        {
            m_selection.erase_first_unsorted( pItem );
            NotifySelectionChanged();
        }
    }

    void TreeListView::HandleItemSelection( TreeListViewItem* pItem, bool isSelectedItem )
    {
        // Left click follows regular selection logic
        if ( ImGui::IsItemClicked( ImGuiMouseButton_Left ) )
        {
            if ( m_multiSelectionAllowed && ImGui::GetIO().KeyShift )
            {
                // TODO
                // Find selection bounds and bulk select everything between them
            }
            else  if ( m_multiSelectionAllowed && ImGui::GetIO().KeyCtrl )
            {
                if ( isSelectedItem )
                {
                    RemoveFromSelection( pItem );
                }
                else
                {
                    AddToSelection( pItem );
                }
            }
            else
            {
                SelectItem( pItem );
            }
        }
        // Right click never deselects! Nor does it support multi-selection
        else if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
        {
            if ( !isSelectedItem )
            {
                SelectItem( pItem );
            }
        }
    }

    //-------------------------------------------------------------------------

    void TreeListView::DrawVisualItem( VisualTreeItem& visualTreeItem )
    {
        KRG_ASSERT( visualTreeItem.m_pItem != nullptr && visualTreeItem.m_hierarchyLevel >= 0 );

        TreeListViewItem* const pItem = visualTreeItem.m_pItem;
        bool const isSelectedItem = VectorContains( m_selection, pItem );
        bool const isActiveItem = ( pItem == m_pActiveItem );

        ImGui::PushID( pItem );
        ImGui::TableNextRow();

        // Draw label column
        //-------------------------------------------------------------------------

        ImGui::TableSetColumnIndex( 0 );

        // Set tree indent level
        float const indentLevel = visualTreeItem.m_hierarchyLevel * ImGui::GetStyle().IndentSpacing;
        bool const requiresIndent = indentLevel > 0;
        if ( requiresIndent )
        {
            ImGui::Indent( indentLevel );
        }

        // Set node flags
        uint32 treeNodeflags = ImGuiTreeNodeFlags_SpanFullWidth;

        if ( m_expandItemsOnlyViaArrow )
        {
            treeNodeflags |= ImGuiTreeNodeFlags_OpenOnArrow;
        }

        if ( isSelectedItem )
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
        TreeListViewItem::ItemState const state = isActiveItem ? TreeListViewItem::Active : isSelectedItem ? TreeListViewItem::Selected : TreeListViewItem::None;
        
        {
            ImGuiX::ScopedFont font( isActiveItem ? ImGuiX::Font::SmallBold : ImGuiX::Font::Small, pItem->GetDisplayColor( state ) );
            newExpansionState = ImGui::TreeNodeEx( pItem->GetDisplayName(), treeNodeflags );
        }

        if ( pItem->SupportsDragAndDrop() )
        {
            if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ) )
            {
                auto payloadData = pItem->GetDragAndDropPayload();
                ImGui::SetDragDropPayload( pItem->GetDragAndDropPayloadID(), payloadData.first, payloadData.second );
                ImGui::Text( pItem->GetDisplayName() );
                ImGui::EndDragDropSource();
            }
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
        HandleItemSelection( pItem, isSelectedItem );

        // Context Menu
        if ( pItem->HasContextMenu() )
        {
            if ( ImGui::BeginPopupContextItem( "ItemContextMenu" ) )
            {
                // is this needed?
                if ( !isSelectedItem )
                {
                    if ( m_multiSelectionAllowed && ImGui::GetIO().KeyShift || ImGui::GetIO().KeyCtrl )
                    {
                        AddToSelection( pItem );
                    }
                    else // Switch selection to this item
                    {
                        SelectItem( pItem );
                    }
                }

                TVector<TreeListViewItem*> selectedItemsWithContextMenus;
                for ( auto pSelectedItem : m_selection )
                {
                    if ( pSelectedItem->HasContextMenu() )
                    {
                        selectedItemsWithContextMenus.emplace_back( pSelectedItem );
                    }
                }

                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().ItemSpacing.x ) ); // Undo the table item spacing changes
                DrawItemContextMenu( selectedItemsWithContextMenus );
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
        if ( m_visualTreeState != VisualTreeState::UpToDate )
        {
            RebuildVisualTree();
        }

        if ( m_visualTree.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( ImGui::GetStyle().ItemSpacing.x, 0 ) ); // Ensure table border and scrollbar align
        ImGui::BeginChild( "TreeViewChild", ImVec2( 0, 0 ), false, 0 );
        {
            constexpr ImGuiTableFlags const tableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
            float const totalVerticalSpaceAvailable = ImGui::GetContentRegionAvail().y;
            float const maxVerticalScrollPosition = ImGui::GetScrollMaxY();

            //-------------------------------------------------------------------------

            // Calculate the required height in pixels for all rows based on the height of the first row. 
            // We only render a single row here (so we dont bust the imgui index buffer limits for a single tree) - At low frame rates this will flicker
            if ( m_estimatedTreeHeight < 0 )
            {
                ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
                ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Style::s_itemColorMedium.Value );
                if ( ImGui::BeginTable( "TreeViewTable", GetNumExtraColumns() + 1, tableFlags, ImVec2( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, 0 ) ) )
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
                if ( ImGui::BeginTable( "Tree View Browser", GetNumExtraColumns() + 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, 0)) )
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

        //-------------------------------------------------------------------------

        DrawAdditionalUI();

        ImGui::PopID();
    }
}