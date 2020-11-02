#include "EntityComponentPickerWidget.h"
#include "EditorContext.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/SearchBoxWidget.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "QLineEdit"
#include "QPushButton"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityComponentPicker
    {
        TreeItem::TreeItem( TypeSystem::TypeInfo const* pTypeInfo, QString const& simpleTypeName )
            : m_pTypeInfo( pTypeInfo )
            , m_name( simpleTypeName )
        {
            KRG_ASSERT( m_pTypeInfo != nullptr && !simpleTypeName.isEmpty() );
        }

        TreeItem::TreeItem( QString const& name )
            : m_name( name )
        {}

        TreeItem::~TreeItem()
        {
            for ( auto pChild : m_children )
            {
                delete pChild;
            }

            m_children.clear();
        }

        void TreeItem::AddChild( TreeItem* pItem )
        {
            KRG_ASSERT( pItem->GetParent() == nullptr );
            pItem->m_pParent = this;
            m_children.emplace_back( pItem );
        }

        TreeItem* TreeItem::GetChild( S32 row )
        {
            KRG_ASSERT( row >= 0 && row < m_children.size() );
            return m_children[row];
        }

        TreeItem* TreeItem::GetChild( QString const& name ) const
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

        S32 TreeItem::GetNumChildren() const
        {
            return (S32) m_children.size();
        }

        S32 TreeItem::GetRowIndex() const
        {
            if ( m_pParent != nullptr )
            {
                S32 const row = VectorFindIndex( m_pParent->m_children, const_cast<TreeItem*>( this ) );
                KRG_ASSERT( row != InvalidIndex );
                return row;
            }

            return 0;
        }

        QVariant TreeItem::GetDataForColumn( S32 column ) const
        {
            return m_name;
        }

        QIcon TreeItem::GetIcon() const
        {
            if ( IsCategory() )
            {
                return KIcon( MaterialDesign::Folder );
            }
            else
            {
                if ( m_pTypeInfo->IsDerivedFrom<SpatialEntityComponent>() )
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

        Model::Model( TypeSystem::TypeRegistry const& typeRegistry, QObject* pParent )
            : QAbstractItemModel( pParent )
        {
            m_pRootItem = new TreeItem( "KRG" );

            //-------------------------------------------------------------------------

            auto const componentTypeInfos = typeRegistry.GetAllTypesWithMatchingMetadata( TFlags<TypeSystem::ETypeInfoMetaData>( TypeSystem::ETypeInfoMetaData::EntityComponent ) );
            for ( auto pComponentTypeInfo : componentTypeInfos )
            {
                TreeItem* pCategoryForItem = m_pRootItem;

                QStringList const path = QString( pComponentTypeInfo->GetTypeName() ).split( "::", Qt::SkipEmptyParts );
                S32 const numElements = (S32) path.size();
                KRG_ASSERT( numElements > 1 || path.front() != "KRG" ); // Type reflector enforces this

                for ( S32 i = 1; i < ( numElements - 1 ); i++ )
                {
                    pCategoryForItem = FindOrAddCategory( pCategoryForItem, path[i] );
                    KRG_ASSERT( pCategoryForItem != nullptr );
                }

                pCategoryForItem->AddChild( new TreeItem( pComponentTypeInfo, path.back() ) );
            }
        }

        Model::~Model()
        {
            delete m_pRootItem;
        }

        TreeItem* Model::FindOrAddCategory( TreeItem* pParentCategory, QString const& categoryName )
        {
            // If we didn't find the category, create it
            TreeItem* pFoundCategory = pParentCategory->GetChild( categoryName );
            if ( pFoundCategory == nullptr )
            {
                pFoundCategory = new TreeItem( categoryName );
                pParentCategory->AddChild( pFoundCategory );
            }

            return pFoundCategory;
        }

        QModelIndex Model::index( S32 row, S32 column, QModelIndex const& parentIndex ) const
        {
            KRG_ASSERT( hasIndex( row, column, parentIndex ) );

            TreeItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<TreeItem*>( parentIndex.internalPointer() );
            }

            TreeItem* pChildItem = pParentItem->GetChild( row );
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

            TreeItem* pChildItem = static_cast<TreeItem*>( index.internalPointer() );
            TreeItem* pParentItem = pChildItem->GetParent();

            if ( pParentItem == m_pRootItem )
            {
                return QModelIndex();
            }

            return createIndex( pParentItem->GetRowIndex(), 0, pParentItem );
        }

        S32 Model::rowCount( QModelIndex const& parentIndex ) const
        {
            TreeItem* pParentItem = nullptr;
            if ( !parentIndex.isValid() )
            {
                pParentItem = m_pRootItem;
            }
            else
            {
                pParentItem = static_cast<TreeItem*>( parentIndex.internalPointer() );
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

            TreeItem *item = static_cast<TreeItem*>( index.internalPointer() );

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

        FilterModel::FilterModel( QObject* pParent )
            : QSortFilterProxyModel( pParent )
        {
            sort( 0, Qt::AscendingOrder );
        }

        void FilterModel::SetFilterString( QString const& filterString )
        {
            m_filter = filterString.split( ' ', Qt::SplitBehaviorFlags::SkipEmptyParts );
            invalidateFilter();
        }

        bool FilterModel::filterAcceptsRow( S32 sourceRow, QModelIndex const& sourceParentIndex ) const
        {
            auto pModel = static_cast<Model*>( sourceModel() );
            auto pItem = static_cast<TreeItem*>( pModel->index( sourceRow, 0, sourceParentIndex ).internalPointer() );
            KRG_ASSERT( pItem != nullptr );

            if ( pItem->IsCategory() )
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
            auto pLeftItem = static_cast<TreeItem*>( left.internalPointer() );
            auto pRightItem = static_cast<TreeItem*>( right.internalPointer() );

            // Categories are always before types
            if ( pLeftItem->IsCategory() )
            {
                // Right item is component type
                if ( !pRightItem->IsCategory() )
                {
                    return true;
                }
            }
            else // Left item is component type
            {
                if ( pRightItem->IsCategory() )
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

    EntityComponentPickerWidget::EntityComponentPickerWidget( TypeSystem::TypeRegistry const& typeRegistry )
        : QWidget()
        , m_model( typeRegistry, this )
        , m_filterModel( this )
    {
        setAttribute( Qt::WA_DeleteOnClose, true );

        // Create filter box
        //-------------------------------------------------------------------------

        auto OnFilterChanged = [this] ( QString const& filterText )
        {
            m_filterModel.SetFilterString( filterText );
        };

        m_pFilterBox = new KSearchBoxWidget();
        connect( m_pFilterBox, &KSearchBoxWidget::SearchInputChanged, this, OnFilterChanged );

        // Create component tree
        //-------------------------------------------------------------------------

        m_filterModel.setSourceModel( &m_model );

        auto pSelectionModel = new QItemSelectionModel( &m_filterModel, this );
        connect( pSelectionModel, &QItemSelectionModel::currentChanged, this, &EntityComponentPickerWidget::OnSelectionChanged );

        m_pTree = new QTreeView();
        m_pTree->setHeaderHidden( true );
        m_pTree->setModel( &m_filterModel );
        m_pTree->setItemsExpandable( false );
        m_pTree->expandAll();
        m_pTree->setSelectionMode( QAbstractItemView::SingleSelection );
        m_pTree->setSelectionModel( pSelectionModel );
        m_pTree->setRootIsDecorated( false );

        //-------------------------------------------------------------------------

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setSpacing( 2 );
        pMainLayout->setMargin( 0 );
        pMainLayout->addWidget( m_pFilterBox, 0 );
        pMainLayout->addWidget( m_pTree, 1 );
        setLayout( pMainLayout );
    }

    void EntityComponentPickerWidget::focusInEvent( QFocusEvent* pEvent )
    {
        QWidget::focusInEvent( pEvent );
        ResetPicker();
        m_pFilterBox->setFocus();
    }

    void EntityComponentPickerWidget::ResetPicker()
    {
        m_pFilterBox->Clear();
        m_pTree->clearSelection();
    }

    void EntityComponentPickerWidget::OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous )
    {
        if ( current.isValid() )
        {
            // Since we are working on the filter model, we need the actual item index from the actual model
            auto actualCurrentIndex = m_filterModel.mapToSource( current );
            if ( actualCurrentIndex.isValid() )
            {
                auto pSelectedItem = static_cast<EntityComponentPicker::TreeItem*>( actualCurrentIndex.internalPointer() );
                if ( !pSelectedItem->IsCategory() )
                {
                    emit OnComponentSelected( pSelectedItem->GetTypeInfo() );
                    ResetPicker();
                }
            }
        }
    }
}