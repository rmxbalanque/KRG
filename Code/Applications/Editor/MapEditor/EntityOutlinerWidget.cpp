#include "EntityOutlinerWidget.h"
#include "EditorContext.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/SearchBoxWidget.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "QLineEdit"
#include "QPushButton"
#include "QBoxLayout"
#include "Tools/Entity/ToolEntityCollection.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityOutliner
    {
        EntityItem::EntityItem( EntityModel::ToolEntity* pEntity )
            : m_pEntity( pEntity )
            , m_name( pEntity->GetNameAsCStr() )
        {
            KRG_ASSERT( pEntity != nullptr );

            for ( auto pChildEntity : pEntity->GetChildEntities() )
            {
                AddChild( new EntityItem( pChildEntity ) );
            }
        }

        EntityItem::EntityItem( QString const& name )
            : m_name( name )
        {
            KRG_ASSERT( !name.isEmpty() );
        }

        EntityItem::~EntityItem()
        {
            for ( auto pChild : m_children )
            {
                delete pChild;
            }

            m_children.clear();
        }

        void EntityItem::AddChild( EntityItem* pItem )
        {
            KRG_ASSERT( pItem->GetParent() == nullptr );
            pItem->m_pParent = this;
            m_children.emplace_back( pItem );
        }

        EntityItem* EntityItem::GetChild( int32 row )
        {
            KRG_ASSERT( row >= 0 && row < m_children.size() );
            return m_children[row];
        }

        EntityItem* EntityItem::GetChild( QString const& name ) const
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

        int32 EntityItem::GetNumChildren() const
        {
            return (int32) m_children.size();
        }

        int32 EntityItem::GetRowIndex() const
        {
            if ( m_pParent != nullptr )
            {
                int32 const row = VectorFindIndex( m_pParent->m_children, const_cast<EntityItem*>( this ) );
                KRG_ASSERT( row != InvalidIndex );
                return row;
            }

            return 0;
        }

        QVariant EntityItem::GetDataForColumn( int32 column ) const
        {
            return m_name;
        }

        QIcon EntityItem::GetIcon() const
        {
            if ( IsFolder() )
            {
                return KIcon( MaterialDesign::Folder );
            }
            else
            {
                if ( m_pEntity->IsSpatialEntity() )
                {
                    return KIcon( MaterialDesign::Earth, QColor( 255, 0, 255 ) );
                }
                else
                {
                    return KIcon( MaterialDesign::Cube_Outline, QColor( 84, 226, 255 ) );
                }
            }
        }

        //-------------------------------------------------------------------------
        // Model
        //-------------------------------------------------------------------------

        Model::Model()
            : QAbstractItemModel()
        {
            m_pRootItem = new EntityItem( "KRG" );
        }

        Model::~Model()
        {
            delete m_pRootItem;
        }

        EntityItem* Model::FindOrAddFolder( EntityItem* pParentCategory, QString const& categoryName )
        {
            // If we didn't find the category, create it
            EntityItem* pFoundFolder = pParentCategory->GetChild( categoryName );
            if ( pFoundFolder == nullptr )
            {
                pFoundFolder = new EntityItem( categoryName );
                pParentCategory->AddChild( pFoundFolder );
            }

            return pFoundFolder;
        }

        void Model::SetCollection( EntityModel::ToolEntityCollection* pCollection )
        {
            m_pEntityCollection = pCollection;

            beginResetModel();

            //-------------------------------------------------------------------------

            delete m_pRootItem;
            m_pRootItem = new EntityItem( "KRG" );

            for ( auto pEntity : m_pEntityCollection->GetEntities() )
            {
                m_pRootItem->AddChild( new EntityItem( pEntity ) );
            }

            //-------------------------------------------------------------------------

            endResetModel();
        }

        QModelIndex Model::index( int32 row, int32 column, QModelIndex const& parentIndex ) const
        {
            KRG_ASSERT( hasIndex( row, column, parentIndex ) );

            EntityItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<EntityItem*>( parentIndex.internalPointer() );
            }

            EntityItem* pChildItem = pParentItem->GetChild( row );
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

            EntityItem* pChildItem = static_cast<EntityItem*>( index.internalPointer() );
            EntityItem* pParentItem = pChildItem->GetParent();

            if ( pParentItem == m_pRootItem )
            {
                return QModelIndex();
            }

            return createIndex( pParentItem->GetRowIndex(), 0, pParentItem );
        }

        int32 Model::rowCount( QModelIndex const& parentIndex ) const
        {
            EntityItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<EntityItem*>( parentIndex.internalPointer() );
            }

            return pParentItem->GetNumChildren();
        }

        int32 Model::columnCount( QModelIndex const& parent ) const
        {
            return 1;
        }

        QVariant Model::data( QModelIndex const& index, int32 role ) const
        {
            if ( !index.isValid() )
            {
                return QVariant();
            }

            //-------------------------------------------------------------------------

            EntityItem *item = static_cast<EntityItem*>( index.internalPointer() );

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

        QVariant Model::headerData( int32 column, Qt::Orientation orientation, int32 role ) const
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

        bool FilterModel::filterAcceptsRow( int32 sourceRow, QModelIndex const& sourceParentIndex ) const
        {
            auto pModel = static_cast<Model*>( sourceModel() );
            auto pItem = static_cast<EntityItem*>( pModel->index( sourceRow, 0, sourceParentIndex ).internalPointer() );
            KRG_ASSERT( pItem != nullptr );

            if ( pItem->IsFolder() )
            {
                return true;
            }

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
            auto pLeftItem = static_cast<EntityItem*>( left.internalPointer() );
            auto pRightItem = static_cast<EntityItem*>( right.internalPointer() );

            // Categories are always before types
            if ( pLeftItem->IsFolder() )
            {
                // Right item is component type
                if ( !pRightItem->IsFolder() )
                {
                    return true;
                }
            }
            else // Left item is component type
            {
                if ( pRightItem->IsFolder() )
                {
                    return false;
                }
            }

            // Simple name sort
            return pLeftItem->GetName() < pRightItem->GetName();;
        }
    }

    //-------------------------------------------------------------------------
    // WIDGET
    //-------------------------------------------------------------------------

    EntityOutlinerWidget::EntityOutlinerWidget()
        : QWidget()
    {
        setAttribute( Qt::WA_DeleteOnClose, true );

        // Create filter box
        //-------------------------------------------------------------------------

        auto OnFilterChanged = [this] ( QString const& filterText )
        {
            m_filterModel.SetFilterString( filterText );
            m_pTree->expandAll();
        };

        m_pFilterBox = new KSearchBoxWidget();
        connect( m_pFilterBox, &KSearchBoxWidget::SearchInputChanged, this, OnFilterChanged );

        // Create component tree
        //-------------------------------------------------------------------------

        m_filterModel.setSourceModel( &m_model );

        auto pSelectionModel = new QItemSelectionModel( &m_filterModel, this );
        connect( pSelectionModel, &QItemSelectionModel::currentChanged, this, &EntityOutlinerWidget::OnSelectionChanged );

        m_pTree = new QTreeView();
        m_pTree->setModel( &m_filterModel );
        m_pTree->setHeaderHidden( true );
        m_pTree->setSelectionMode( QAbstractItemView::SingleSelection );
        m_pTree->setSelectionModel( pSelectionModel );

        //-------------------------------------------------------------------------

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setSpacing( 2 );
        pMainLayout->setMargin( 0 );
        pMainLayout->addWidget( m_pFilterBox, 0 );
        pMainLayout->addWidget( m_pTree, 1 );
        setLayout( pMainLayout );
    }

    void EntityOutlinerWidget::SetCollectionToOutline( EntityModel::ToolEntityCollection* pCollection )
    {
        m_model.SetCollection( pCollection );
        m_pTree->expandAll();
    }

    void EntityOutlinerWidget::focusInEvent( QFocusEvent* pEvent )
    {
        QWidget::focusInEvent( pEvent );
        m_pFilterBox->setFocus();
    }

    void EntityOutlinerWidget::OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous )
    {
        if ( current.isValid() )
        {
            // Since we are working on the filter model, we need the actual item index from the actual model
            auto actualCurrentIndex = m_filterModel.mapToSource( current );
            if ( actualCurrentIndex.isValid() )
            {
                auto pSelectedItem = static_cast<EntityOutliner::EntityItem*>( actualCurrentIndex.internalPointer() );
                if ( !pSelectedItem->IsFolder() )
                {
                    emit OnEntitySelected( pSelectedItem->GetEntity() );
                }
            }
        }
    }
}