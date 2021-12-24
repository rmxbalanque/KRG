#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Types/Function.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_CORE_API TreeListViewItem
    {
        friend class TreeListView;

    public:

        TreeListViewItem() = default;

        explicit TreeListViewItem( String const& label, int32 hierarchyLevel )
            : m_nameID( label )
            , m_hierarchyLevel( hierarchyLevel )
        {}

        explicit TreeListViewItem( char const* pLabel, int32 hierarchyLevel )
            : m_nameID( pLabel )
            , m_hierarchyLevel( hierarchyLevel )
        {}

        virtual ~TreeListViewItem() = default;

        //-------------------------------------------------------------------------

        virtual uint32 GetUniqueID() const = 0;
        inline StringID GetNameID() const { return m_nameID; }
        virtual char const* GetDisplayName() const { return m_nameID.c_str(); }
        virtual ImVec4 GetDisplayColor( bool isActive = false ) const;
        inline int32 GetHierarchyLevel() const { return m_hierarchyLevel; }
        virtual bool HasContextMenu() const { return false; }
        virtual bool IsActivatable() const { return true; }

        // Expansion
        //-------------------------------------------------------------------------

        inline void SetExpanded( bool isExpanded ) { m_isExpanded = isExpanded && HasChildren(); }
        inline bool IsExpanded() const { return m_isExpanded; }

        // Visibility
        //-------------------------------------------------------------------------

        inline bool IsVisible() const { return m_isVisible; }
        inline bool HasVisibleChildren() const { return !m_children.empty(); }

        // Update visibility for this branch based on a user-supplied delegate
        void UpdateVisibility( TFunction<bool( TreeListViewItem const* pItem )> const& isVisibleFunction, bool showParentItemsWithNoVisibleChildren = false );

        // Children
        //-------------------------------------------------------------------------

        inline bool HasChildren() const { return !m_children.empty(); }
        inline TVector<TreeListViewItem*> const& GetChildren() { return m_children; }
        inline TVector<TreeListViewItem*> const& GetChildren() const { return m_children; }

        // Create a new child tree view item (memory is owned by this item)
        template< typename T, typename ... ConstructorParams >
        T* CreateChild( ConstructorParams&&... params )
        {
            static_assert( std::is_base_of<TreeListViewItem, T>::value, "T must derive from TreeViewItem" );
            TreeListViewItem* pAddedItem = m_children.emplace_back( KRG::New<T>( std::forward<ConstructorParams>( params )... ) );
            KRG_ASSERT( pAddedItem->GetUniqueID() != 0 );
            return static_cast<T*>( pAddedItem );
        }

        // Destroys all child for this branch
        void DestroyChildren();

        // Find a specific child
        TreeListViewItem* FindChild( TFunction<bool( TreeListViewItem const* )> const& searchPredicate );

        // Recursively search all children
        TreeListViewItem* SearchChildren( TFunction<bool( TreeListViewItem const* )> const& searchPredicate );

        // Apply some operation for all elements in this branch
        inline void ForEachChild( TFunction<void( TreeListViewItem* pItem )> const& function )
        {
            for ( auto& pChild : m_children )
            {
                function( pChild );
                pChild->ForEachChild( function );
            }
        }

        // Apply some operation for all elements in this branch
        inline void ForEachChildConst( TFunction<void( TreeListViewItem const* pItem )> const& function ) const
        {
            for ( auto& pChild : m_children )
            {
                function( pChild );
                pChild->ForEachChildConst( function );
            }
        }

    private:

        // Disable copies/moves
        TreeListViewItem& operator=( TreeListViewItem const& ) = delete;
        TreeListViewItem& operator=( TreeListViewItem&& ) = delete;

    protected:

        StringID                                m_nameID;
        TVector<TreeListViewItem*>              m_children;
        int32                                   m_hierarchyLevel = -1;
        bool                                    m_isVisible = true;
        bool                                    m_isExpanded = false;
    };

    //-------------------------------------------------------------------------
    // Tree List View 
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API TreeListView
    {
        enum class VisualTreeState
        {
            None,
            UpToDate,
            NeedsRebuild,
            NeedsRebuildAndViewReset
        };

    public:

        TreeListView() = default;
        virtual ~TreeListView();

        // Visual
        //-------------------------------------------------------------------------

        void Draw();
        inline void RefreshVisualState() { m_visualTreeState = VisualTreeState::NeedsRebuild; }

        // Selection, Activation and Events
        //-------------------------------------------------------------------------

        inline TreeListViewItem* GetSelectedItem() const { return m_pSelectedItem; }

        template<typename T>
        inline T* GetSelectedItem() const { return static_cast<T*>( m_pSelectedItem ); }

        // Fire whenever the selection changes
        inline TEventHandle<> OnSelectedChanged() { return m_onSelectionChanged; }

        // Fires whenever the active item changes, parameter is the new active item (can be null)
        inline TEventHandle<TreeListViewItem*> OnActiveItemChanged() { return m_onActiveItemChanged; }

        // Fires whenever an item is double clicked, parameter is the item that was double clicked (cant be null)
        inline TEventHandle<TreeListViewItem*> OnItemDoubleClicked() { return m_onItemDoubleClicked; }

        // Bulk Item Operations
        //-------------------------------------------------------------------------

        void ForEachItem( TFunction<void( TreeListViewItem* pItem )> const& function, bool refreshVisualState = true )
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

        void ForEachItemConst( TFunction<void( TreeListViewItem const* pItem )> const& function ) const
        {
            if ( m_pRoot != nullptr )
            {
                m_pRoot->ForEachChildConst( function );
            }
        }

        void UpdateItemVisibility( TFunction<bool( TreeListViewItem const* pItem )> const& isVisibleFunction, bool showParentItemsWithNoVisibleChildren = false )
        {
            if ( m_pRoot != nullptr )
            {
                m_pRoot->UpdateVisibility( isVisibleFunction, showParentItemsWithNoVisibleChildren );
                RefreshVisualState();
            }
        }

    protected:

        inline int32 GetNumItems() const { return (int32) m_visualTree.size(); }

        // User overrideable function to draw any addition windows/dialogs that might be needed
        virtual void DrawAdditionalUI() {}

        // Get the number of extra columns needed
        virtual uint32 GetNumExtraColumns() const { return 0; }

        // Get the number of extra columns needed
        virtual void SetupExtraColumnHeaders() const {}

        // Draw any custom item controls you might need
        virtual void DrawItemExtraColumns( TreeListViewItem* pItem, int32 extraColumnIdx ) {}

        // Draw any custom item context menus you might need
        virtual void DrawItemContextMenu( TreeListViewItem* pItem ) {}

    private:

        void DrawVisualItem( TreeListViewItem* pItem );
        void TryAddItemToVisualTree( TreeListViewItem* pItem );
        void UpdateVisualTree();

        void OnItemDoubleClickedInternal( TreeListViewItem* pItem );

    protected:

        TreeListViewItem*                                       m_pRoot = nullptr;

        TEvent<>                                                m_onSelectionChanged;
        TEvent<TreeListViewItem*>                               m_onActiveItemChanged;
        TEvent<TreeListViewItem*>                               m_onItemDoubleClicked;

        // The active item is an item that is activated (and deactivated) via a double click
        TreeListViewItem*                                       m_pActiveItem = nullptr;

        // The currently selected item (changes frequently due to clicks/focus/etc...)
        TreeListViewItem*                                       m_pSelectedItem = nullptr;

    private:

        TVector<TreeListViewItem*>                              m_visualTree;
        VisualTreeState                                         m_visualTreeState = VisualTreeState::None;
        float                                                   m_estimatedRowHeight = -1.0f;
        float                                                   m_estimatedTreeHeight = -1.0f;
        int32                                                   m_firstVisibleRowItemIdx = 0;
        float                                                   m_itemControlColumnWidth = 0;
        bool                                                    m_maintainVisibleRowIdx = false;
    };
}