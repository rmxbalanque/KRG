#include "EntityComponentEditorWidget.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/SearchBoxWidget.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "QSplitter"
#include "QBoxLayout"
#include "EditorContext.h"
#include "QScrollArea"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityComponentTree
    {
        ComponentItem::ComponentItem( ToolEntityComponent* pComponent )
            : m_pComponent( pComponent )
            , m_name( pComponent->GetNameAsCStr() )
        {
            KRG_ASSERT( pComponent != nullptr );

            for ( auto& childComponent : pComponent->GetChildComponents() )
            {
                AddChild( new ComponentItem( &childComponent ) );
            }
        }

        ComponentItem::~ComponentItem()
        {
            for ( auto pChild : m_children )
            {
                delete pChild;
            }

            m_children.clear();
        }

        void ComponentItem::AddChild( ComponentItem* pItem )
        {
            KRG_ASSERT( pItem->GetParent() == nullptr );
            pItem->m_pParent = this;
            m_children.emplace_back( pItem );
        }

        ComponentItem* ComponentItem::GetChild( S32 row )
        {
            KRG_ASSERT( row >= 0 && row < m_children.size() );
            return m_children[row];
        }

        ComponentItem* ComponentItem::GetChild( QString const& name ) const
        {
            for ( auto pChild : m_children )
            {
                if ( pChild->m_name == name )
                {
                    return pChild;
                }
            }

            return nullptr;
        }

        S32 ComponentItem::GetNumChildren() const
        {
            return (S32) m_children.size();
        }

        S32 ComponentItem::GetRowIndex() const
        {
            if ( m_pParent != nullptr )
            {
                S32 const row = VectorFindIndex( m_pParent->m_children, const_cast<ComponentItem*>( this ) );
                KRG_ASSERT( row != InvalidIndex );
                return row;
            }

            return 0;
        }

        QVariant ComponentItem::GetDataForColumn( S32 column ) const
        {
            return m_name;
        }

        QIcon ComponentItem::GetIcon() const
        {
            if ( m_pComponent->IsSpatialComponent() )
            {
                return KIcon( MaterialDesign::Earth, QColor( 255, 0, 255 ) );
            }
            else
            {
                return KIcon( MaterialDesign::Cube_Outline, QColor( 84, 226, 255 ) );
            }
        }

        //-------------------------------------------------------------------------
        // MODEL
        //-------------------------------------------------------------------------

        Model::Model()
            : QAbstractItemModel()
        {
            m_pRootItem = new ComponentItem();
        }

        Model::~Model()
        {
            delete m_pRootItem;
        }

        void Model::SetEntityToEdit( ToolEntity* pEntity )
        {
            m_pEntity = pEntity;

            //-------------------------------------------------------------------------

            beginResetModel();

            //-------------------------------------------------------------------------

            delete m_pRootItem;
            m_pRootItem = new ComponentItem();

            //-------------------------------------------------------------------------

            for ( ToolEntityComponent& component : m_pEntity->GetComponents() )
            {
                m_pRootItem->AddChild( new ComponentItem( &component ) );
            }

            //-------------------------------------------------------------------------

            endResetModel();
        }

        QModelIndex Model::index( S32 row, S32 column, QModelIndex const& parentIndex ) const
        {
            if( !hasIndex( row, column, parentIndex ) )
            {
                return QModelIndex();
            }

            ComponentItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<ComponentItem*>( parentIndex.internalPointer() );
            }

            ComponentItem* pChildItem = pParentItem->GetChild( row );
            if ( pChildItem )
            {
                return createIndex( row, column, pChildItem );
            }

            return QModelIndex();
        }

        QModelIndex Model::parent( QModelIndex const& index ) const
        {
            if ( !index.isValid() )
            {
                return QModelIndex();
            }

            ComponentItem* pChildItem = static_cast<ComponentItem*>( index.internalPointer() );
            ComponentItem* pParentItem = pChildItem->GetParent();

            if ( pParentItem == m_pRootItem )
            {
                return QModelIndex();
            }

            return createIndex( pParentItem->GetRowIndex(), 0, pParentItem );
        }

        S32 Model::rowCount( QModelIndex const& parentIndex ) const
        {
            ComponentItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<ComponentItem*>( parentIndex.internalPointer() );
            }

            return pParentItem->GetNumChildren();
        }

        S32 Model::columnCount( QModelIndex const& parent ) const
        {
            return 1;
        }

        QVariant Model::data( QModelIndex const& index, S32 role ) const
        {
            if ( !index.isValid() )
            {
                return QVariant();
            }

            //-------------------------------------------------------------------------

            ComponentItem *item = static_cast<ComponentItem*>( index.internalPointer() );

            if ( role == Qt::DecorationRole )
            {
                return item->GetIcon();
            }
            if ( role == Qt::DisplayRole )
            {
                return item->GetDataForColumn( index.column() );
            }

            return QVariant();
        }

        Qt::ItemFlags Model::flags( QModelIndex const& index ) const
        {
            if ( !index.isValid() )
            {
                return Qt::NoItemFlags;
            }

            return QAbstractItemModel::flags( index );
        }

        QVariant Model::headerData( S32 column, Qt::Orientation orientation, S32 role ) const
        {
            if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
            {
                return m_pRootItem->GetDataForColumn( column );
            }

            return QVariant();
        }

        //-------------------------------------------------------------------------
        // FILTER
        //-------------------------------------------------------------------------

        FilterModel::FilterModel()
            : QSortFilterProxyModel()
        {
            sort( 0, Qt::AscendingOrder );
        }

        void FilterModel::SetFilterString( QString const& filterString )
        {
            m_filter = filterString.split( ' ', Qt::SplitBehaviorFlags::SkipEmptyParts );
            invalidateFilter();
        }

        void FilterModel::ClearFilterString()
        {
            m_filter.clear();
            invalidateFilter();
        }

        bool FilterModel::filterAcceptsRow( S32 sourceRow, QModelIndex const& sourceParentIndex ) const
        {
            auto pModel = static_cast<Model*>( sourceModel() );
            auto pItem = static_cast<ComponentItem*>( pModel->index( sourceRow, 0, sourceParentIndex ).internalPointer() );
            KRG_ASSERT( pItem != nullptr );

            for ( auto const& filterElement : m_filter )
            {
                if ( !pItem->GetName().contains( filterElement, Qt::CaseInsensitive ) )
                {
                    return false;
                }
            }

            return true;
        }

        bool FilterModel::lessThan( QModelIndex const& left, QModelIndex const& right ) const
        {
            auto pModel = static_cast<Model*>( sourceModel() );
            auto pLeftItem = static_cast<ComponentItem*>( left.internalPointer() );
            auto pRightItem = static_cast<ComponentItem*>( right.internalPointer() );

            // Simple name sort
            return pLeftItem->GetName() < pRightItem->GetName();;
        }
    }

    //-------------------------------------------------------------------------
    // WIDGET
    //-------------------------------------------------------------------------

    EntityComponentEditorWidget::EntityComponentEditorWidget()
        : QWidget()
    {
        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

        //-------------------------------------------------------------------------

        auto OnFilterChanged = [this] ( QString const& filterText )
        {
            m_filterModel.SetFilterString( filterText );
            m_pComponentTree->expandAll();
        };

        m_pFilterBox = new KSearchBoxWidget();
        m_pFilterBox->SetPlaceholderText( "Filter Components..." );
        m_pFilterBox->setVisible( false );

        connect( m_pFilterBox, &KSearchBoxWidget::SearchInputChanged, this, OnFilterChanged );

        //-------------------------------------------------------------------------
        
        m_filterModel.setSourceModel( &m_model );

        auto pSelectionModel = new QItemSelectionModel( &m_filterModel, this );
        connect( pSelectionModel, &QItemSelectionModel::currentChanged, this, &EntityComponentEditorWidget::OnSelectionChanged );

        m_pComponentTree = new QTreeView();
        m_pComponentTree->setHeaderHidden( true );
        m_pComponentTree->setModel( &m_filterModel );
        m_pComponentTree->setSelectionMode( QAbstractItemView::SingleSelection );
        m_pComponentTree->setSelectionModel( pSelectionModel );
        m_pComponentTree->setVisible( false );

        //-------------------------------------------------------------------------

        m_pEmptyTreeMessage = new QWidget;
        m_pEmptyTreeMessage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

        auto pMessageLayout = new QVBoxLayout( m_pEmptyTreeMessage );
        pMessageLayout->addWidget( new QLabel( "No Entity Selected!" ), 1, Qt::AlignCenter );

        // Layout
        //-------------------------------------------------------------------------

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setSpacing( 2 );
        pMainLayout->setMargin( 0 );
        pMainLayout->addWidget( m_pEmptyTreeMessage, 1, Qt::AlignCenter );
        pMainLayout->addWidget( m_pFilterBox, 0 );
        pMainLayout->addWidget( m_pComponentTree, 1 );
    }

    void EntityComponentEditorWidget::SetEntityToEdit( ToolEntity* pEntity )
    {
        m_pComponentTree->clearSelection();
        m_model.SetEntityToEdit( pEntity );

        if ( pEntity != nullptr )
        {
            m_pEmptyTreeMessage->setVisible( false );
            m_pFilterBox->setVisible( true );
            m_pComponentTree->setVisible( true );
            m_pComponentTree->expandAll();
        }
        else
        {
            m_pFilterBox->setVisible( false );
            m_pComponentTree->setVisible( false );
            m_pEmptyTreeMessage->setVisible( true );
        }
    }

    void EntityComponentEditorWidget::OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous )
    {
        if ( current.isValid() )
        {
            // Since we are working on the filter model, we need the actual item index from the actual model
            auto actualCurrentIndex = m_filterModel.mapToSource( current );
            if ( actualCurrentIndex.isValid() )
            {
                auto pSelectedItem = static_cast<EntityComponentTree::ComponentItem*>( actualCurrentIndex.internalPointer() );
                emit OnComponentSelected( pSelectedItem->GetComponent() );
            }
        }
    }
}