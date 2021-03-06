#pragma once
#include "QTreeView"
#include "Tools/Entity/ToolEntity.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/TypeSystem/TypeInfo.h"
#include "QSortFilterProxyModel"

//-------------------------------------------------------------------------

namespace KRG
{
    struct EditorContext;
    class KSearchBoxWidget;

    namespace EntityModel 
    { 
        class ToolEntity;
        class ToolEntityCollection; 
    }

    //-------------------------------------------------------------------------

    namespace EntityOutliner
    {
        class EntityItem
        {
        public:

            explicit EntityItem( EntityModel::ToolEntity* pEntity );
            explicit EntityItem( QString const& name );
            ~EntityItem();

            inline QString const& GetName() const { return m_name; }
            inline bool IsFolder() const { return m_pEntity == nullptr; }
            inline EntityItem* GetParent() const { return m_pParent; }

            EntityModel::ToolEntity* GetEntity() const { return m_pEntity; }

            int32 GetNumChildren() const;
            EntityItem* GetChild( int32 row );
            EntityItem* GetChild( QString const& name ) const;
            int32 GetRowIndex() const;

            void AddChild( EntityItem* pChild );

            QVariant GetDataForColumn( int32 column ) const;
            QIcon GetIcon() const;

        private:

            EntityModel::ToolEntity*                     m_pEntity = nullptr;
            QString                                      m_name;
            TVector<EntityItem*>                         m_children;
            EntityItem*                                  m_pParent = nullptr;
        };

        //-------------------------------------------------------------------------

        class Model final : public QAbstractItemModel
        {
            Q_OBJECT

        public:

            explicit Model();
            ~Model();

            void SetCollection( EntityModel::ToolEntityCollection* pCollection );

            virtual QVariant data( QModelIndex const& index, int32 role ) const override;
            virtual Qt::ItemFlags flags( QModelIndex const& index ) const override;
            virtual QVariant headerData( int32 section, Qt::Orientation orientation, int32 role = Qt::DisplayRole ) const override;
            virtual QModelIndex index( int32 row, int32 column, QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual QModelIndex parent( QModelIndex const& index ) const override;
            virtual int32 rowCount( QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual int32 columnCount( QModelIndex const& parentIndex = QModelIndex() ) const override;

        private:

            EntityItem* FindOrAddFolder( EntityItem* pParentCategory, QString const& categoryName );

        private:

            EntityModel::ToolEntityCollection*          m_pEntityCollection = nullptr;
            EntityItem*                                 m_pRootItem = nullptr;
        };

        //-------------------------------------------------------------------------

        class FilterModel final : public QSortFilterProxyModel
        {
        public:

            explicit FilterModel();

            void SetFilterString( QString const& filterString );

        private:

            virtual bool filterAcceptsRow( int32 sourceRow, QModelIndex const& sourceParentIndex ) const override final;
            virtual bool lessThan( QModelIndex const& left, QModelIndex const& right ) const override final;

        private:

            QStringList                                     m_filter;
        };
    }

    //-------------------------------------------------------------------------

    class EntityOutlinerWidget final : public QWidget
    {
        Q_OBJECT

    signals:

        void OnEntitySelected( EntityModel::ToolEntity* pSelectedEntity );

    public:

        EntityOutlinerWidget();

        void SetCollectionToOutline( EntityModel::ToolEntityCollection* pCollection );

    private:

        virtual void focusInEvent( QFocusEvent* pEvent ) override final;
        void OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous );

    private:

        KSearchBoxWidget*                                   m_pFilterBox = nullptr;
        QTreeView*                                          m_pTree = nullptr;
        EntityOutliner::Model                               m_model;
        EntityOutliner::FilterModel                         m_filterModel;
    };
}