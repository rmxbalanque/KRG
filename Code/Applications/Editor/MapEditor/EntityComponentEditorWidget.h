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
            explicit ComponentItem( ToolEntityComponent* pComponent );
            ~ComponentItem();

            inline bool IsRootItem() const { return m_pComponent == nullptr; }

            inline QString const& GetName() const { return m_name; }
            inline ToolEntityComponent* GetComponent() const { return m_pComponent; }
            inline bool IsSpatialComponent() const { return m_pComponent->IsSpatialComponent(); }

            inline ComponentItem* GetParent() const { return m_pParent; }
            S32 GetNumChildren() const;
            S32 GetRowIndex() const;

            ComponentItem* GetChild( S32 row );
            ComponentItem* GetChild( QString const& name ) const;
            void AddChild( ComponentItem* pChild );

            QVariant GetDataForColumn( S32 column ) const;
            QIcon GetIcon() const;

        private:

            ToolEntityComponent*                    m_pComponent = nullptr;
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
            void SetEntityToEdit( ToolEntity* pEntity );

            virtual QVariant data( QModelIndex const& index, S32 role ) const override;
            virtual Qt::ItemFlags flags( QModelIndex const& index ) const override;
            virtual QVariant headerData( S32 section, Qt::Orientation orientation, S32 role = Qt::DisplayRole ) const override;
            virtual QModelIndex index( S32 row, S32 column, QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual QModelIndex parent( QModelIndex const& index ) const override;
            virtual S32 rowCount( QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual S32 columnCount( QModelIndex const& parentIndex = QModelIndex() ) const override;

        private:

            ToolEntity*                             m_pEntity = nullptr;
            ComponentItem*                               m_pRootItem = nullptr;
        };

        //-------------------------------------------------------------------------

        class FilterModel final : public QSortFilterProxyModel
        {
        public:

            explicit FilterModel();

            void SetFilterString( QString const& filterString );
            void ClearFilterString();

        private:

            virtual bool filterAcceptsRow( S32 sourceRow, QModelIndex const& sourceParentIndex ) const override final;
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

        void OnComponentSelected( ToolEntityComponent* pSelectedComponent );

    public:

        EntityComponentEditorWidget();

        void SetEntityToEdit( ToolEntity* pEntity );

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
