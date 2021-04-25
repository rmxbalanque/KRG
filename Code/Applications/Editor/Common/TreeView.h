#pragma once

#include "System/DevTools/ImguiX.h"
#include "System/Core/Types/Function.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TreeViewItem
    {
        friend class TreeView;

    public:

        TreeViewItem() = default;

        TreeViewItem( StringID ID, String const& label, int32 hierarchyLevel )
            : m_ID( ID )
            , m_label( label )
            , m_hierarchyLevel( hierarchyLevel )
        {}

        virtual ~TreeViewItem() = default;

        inline StringID const& GetStringID() const { return m_ID; }
        inline char const* GetLabel() const { return m_label.c_str(); }
        inline int32 GetHierarchyLevel() const { return m_hierarchyLevel; }

        // Client API
        //-------------------------------------------------------------------------

        virtual ImVec4 const& GetLabelColor( bool isActive = false ) const { static const ImVec4 activeColor = Colors::GreenYellow.ToFloat4(); return isActive ? activeColor : ImGuiX::Theme::s_textColor; }
        virtual void DrawControls() {}
        virtual bool SupportsContextMenu() const { return false; }
        virtual void DrawContextMenu() {}
        virtual bool CanBeSetActive() const { return true; }

        // Children
        //-------------------------------------------------------------------------

        inline bool HasChildren() const { return !m_children.empty(); }
        inline TVector<TreeViewItem*> const& GetChildren() { return m_children; }
        inline TVector<TreeViewItem*> const& GetChildren() const { return m_children; }

        // Create a new child tree view item (memory is owned by this item)
        template< typename T, typename ... ConstructorParams >
        T* CreateChild( ConstructorParams&&... params )
        {
            static_assert( std::is_base_of<TreeViewItem, T>::value, "T must derive from TreeViewItem" );
            TreeViewItem* pAddedItem = m_children.emplace_back( KRG::New<T>( std::forward<ConstructorParams>( params )... ) );
            return static_cast<T*>( pAddedItem );
        }

        // Destroys all child for this branch
        inline void DestroyChildren()
        {
            for ( auto& pChild : m_children )
            {
                pChild->DestroyChildren();
                KRG::Delete( pChild );
            }

            m_children.clear();
        }

        // Find a specific child
        TreeViewItem* FindChild( TFunction<bool( TreeViewItem const* )> const& searchPredicate )
        {
            TreeViewItem* pFoundItem = nullptr;
            auto itemIter = eastl::find_if( m_children.begin(), m_children.end(), searchPredicate );
            if ( itemIter != m_children.end() )
            {
                pFoundItem = *itemIter;
            }
            return pFoundItem;
        }

        // Apply some operation for all elements in this branch
        inline void ForEachChild( TFunction<void( TreeViewItem* pItem )> const& function )
        {
            for ( auto& pChild : m_children )
            {
                function( pChild );
                pChild->ForEachChild( function );
            }
        }

        // Apply some operation for all elements in this branch
        inline void ForEachChildConst( TFunction<void( TreeViewItem const* pItem )> const& function ) const
        {
            for ( auto& pChild : m_children )
            {
                function( pChild );
                pChild->ForEachChildConst( function );
            }
        }

        // Expansion
        //-------------------------------------------------------------------------

        inline void SetExpanded( bool isExpanded ) { m_isExpanded = isExpanded && HasChildren(); }
        inline bool IsExpanded() const { return m_isExpanded; }

        // Visibility
        //-------------------------------------------------------------------------

        inline bool IsVisible() const { return m_isVisible; }
        inline bool HasVisibleChildren() const { return !m_children.empty(); }

        // Update visibility for this branch based on a user-supplied delegate
        inline void UpdateVisibility( TFunction<bool( TreeViewItem const* pItem )> const& isVisibleFunction, bool showParentItemsWithNoVisibleChildren = false )
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

    private:

        // Disable copies/moves
        TreeViewItem& operator=( TreeViewItem const& ) = delete;
        TreeViewItem& operator=( TreeViewItem&& ) = delete;

    protected:

        StringID                                m_ID;
        String                                  m_label;
        TVector<TreeViewItem*>                  m_children;
        int32                                   m_hierarchyLevel = -1;
        bool                                    m_isVisible = true;
        bool                                    m_isExpanded = false;
    };

    //-------------------------------------------------------------------------
    // Tree View 
    //-------------------------------------------------------------------------
    // Custom retained mode widget Widget to manage selection/clipping etc...

    class TreeView
    {
        enum class VisualTreeState
        {
            None,
            UpToDate,
            NeedsRebuild,
            NeedsRebuildAndViewReset
        };

    public:

        TreeView() = default;
        virtual ~TreeView();

        // Visual
        //-------------------------------------------------------------------------

        void Draw();
        inline void RefreshVisualState() { m_visualTreeState = VisualTreeState::NeedsRebuild; }

        // Selection, Activation and Events
        //-------------------------------------------------------------------------

        inline TreeViewItem* GetSelectedItem() const { return m_pSelectedItem; }

        template<typename T>
        inline T* GetSelectedItem() const { return static_cast<T*>( m_pSelectedItem ); }

        // Fire whenever the selection changes
        inline TSingleUserEvent<void()> OnSelectedChanged() { return m_onSelectionChanged; }

        // Fires whenever the active item changes, parameter is the new active item (can be null)
        inline TSingleUserEvent<void( TreeViewItem* )> OnActiveItemChanged() { return m_onActiveItemChanged; }

        // Fires whenever an item is double clicked, parameter is the item that was double clicked (cant be null)
        inline TSingleUserEvent<void( TreeViewItem* )> OnItemDoubleClicked() { return m_onItemDoubleClicked; }

        // Bulk Item Operations
        //-------------------------------------------------------------------------

        void ForEachItem( TFunction<void( TreeViewItem* pItem )> const& function, bool refreshVisualState = true )
        {
            if ( m_pRoot != nullptr )
            {
                m_pRoot->ForEachChild( function );

                if ( refreshVisualState )
                {
                    RefreshVisualState();
                }
            }
        }

        void ForEachItemConst( TFunction<void( TreeViewItem const* pItem )> const& function ) const
        {
            if ( m_pRoot != nullptr )
            {
                m_pRoot->ForEachChildConst( function );
            }
        }

        void UpdateItemVisibility( TFunction<bool( TreeViewItem const* pItem )> const& isVisibleFunction, bool showParentItemsWithNoVisibleChildren = false )
        {
            if ( m_pRoot != nullptr )
            {
                m_pRoot->UpdateVisibility( isVisibleFunction, showParentItemsWithNoVisibleChildren );
                RefreshVisualState();
            }
        }

    protected:

        inline void SetItemControlColumnWidth( float width )
        {
            KRG_ASSERT( width >= 0 );
            m_itemControlColumnWidth = width; 
        }

    private:

        void DrawVisualItem( TreeViewItem* pItem );
        void TryAddItemToVisualTree( TreeViewItem* pItem );
        void UpdateVisualTree();

        void OnSelectedChangedInternal();
        void OnItemDoubleClickedInternal( TreeViewItem* pItem );

    protected:

        TreeViewItem*                                           m_pRoot = nullptr;

        TSingleUserEventInternal<void()>                        m_onSelectionChanged;
        TSingleUserEventInternal<void( TreeViewItem* )>         m_onActiveItemChanged;
        TSingleUserEventInternal<void( TreeViewItem* )>         m_onItemDoubleClicked;

        // The active item is an item that is activated (and deactivated) via a double click
        TreeViewItem*                                           m_pActiveItem = nullptr;

        // The currently selected item (changes frequently due to clicks/focus/etc...)
        TreeViewItem*                                           m_pSelectedItem = nullptr;

    private:

        TVector<TreeViewItem*>                                  m_visualTree;
        VisualTreeState                                         m_visualTreeState = VisualTreeState::None;
        float                                                   m_estimatedRowHeight = -1.0f;
        float                                                   m_estimatedTreeHeight = -1.0f;
        int32                                                   m_firstVisibleRowItemIdx = 0;
        float                                                   m_itemControlColumnWidth = 0;
        bool                                                    m_maintainVisibleRowIdx = false;
    };
}