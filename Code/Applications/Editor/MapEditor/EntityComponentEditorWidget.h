#pragma once

#include "Tools/Entity/ToolEntity.h"
#include "QWidget"
#include "QTreeView"
#include "QSortFilterProxyModel"

//-------------------------------------------------------------------------

namespace KRG
{
    struct EditorContext;
    class KSearchBoxWidget;

    //-------------------------------------------------------------------------

    namespace EntityComponentTree
    {
        class ComponentItem
        {
        public:

            explicit ComponentItem() = default;
            explicit ComponentItem( EntityModel::ToolEntityComponent* pComponent );
            ~ComponentItem();

            inline bool IsRootItem() const { return m_pComponent == nullptr; }

            inline QString const& GetName() const { return m_name; }
            inline EntityModel::ToolEntityComponent* GetComponent() const { return m_pComponent; }
            inline bool IsSpatialComponent() const { return m_pComponent->IsSpatialComponent(); }

            inline ComponentItem* GetParent() const { return m_pParent; }
            int32 GetNumChildren() const;
            int32 GetRowIndex() const;

            ComponentItem* GetChild( int32 row );
            ComponentItem* GetChild( QString const& name ) const;
            void AddChild( ComponentItem* pChild );

            QVariant GetDataForColumn( int32 column ) const;
            QIcon GetIcon() const;

        private:

            EntityModel::ToolEntityComponent*       m_pComponent = nullptr;
            ComponentItem*                          m_pParent = nullptr;
            QString                                 m_name;
            TVector<ComponentItem*>                 m_children;
        };

        //-------------------------------------------------------------------------

        class Model final : public QAbstractItemModel
        {
            Q_OBJECT

        public:

            explicit Model();
            ~Model();

            inline bool HasEntitySet() const { return m_pEntity != nullptr; }
            void SetEntityToEdit( EntityModel::ToolEntity* pEntity );

            virtual QVariant data( QModelIndex const& index, int32 role ) const override;
            virtual Qt::ItemFlags flags( QModelIndex const& index ) const override;
            virtual QVariant headerData( int32 section, Qt::Orientation orientation, int32 role = Qt::DisplayRole ) const override;
            virtual QModelIndex index( int32 row, int32 column, QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual QModelIndex parent( QModelIndex const& index ) const override;
            virtual int32 rowCount( QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual int32 columnCount( QModelIndex const& parentIndex = QModelIndex() ) const override;

        private:

            EntityModel::ToolEntity*                 m_pEntity = nullptr;
            ComponentItem*                           m_pRootItem = nullptr;
        };

        //-------------------------------------------------------------------------

        class FilterModel final : public QSortFilterProxyModel
        {
        public:

            explicit FilterModel();

            void SetFilterString( QString const& filterString );
            void ClearFilterString();

        private:

            virtual bool filterAcceptsRow( int32 sourceRow, QModelIndex const& sourceParentIndex ) const override final;
            virtual bool lessThan( QModelIndex const& left, QModelIndex const& right ) const override final;

        private:

            QStringList m_filter;
        };
    }
    
    //-------------------------------------------------------------------------

    class EntityComponentEditorWidget : public QWidget
    {
        Q_OBJECT

    signals:

        void OnComponentSelected( EntityModel::ToolEntityComponent* pSelectedComponent );

    public:

        EntityComponentEditorWidget();

        void SetEntityToEdit( EntityModel::ToolEntity* pEntity );

    private:

        void OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous );

    private:

        KSearchBoxWidget*                   m_pFilterBox = nullptr;
        QTreeView*                          m_pComponentTree = nullptr;
        QWidget*                            m_pEmptyTreeMessage = nullptr;
        EntityComponentTree::Model          m_model;
        EntityComponentTree::FilterModel    m_filterModel;
    };

}
