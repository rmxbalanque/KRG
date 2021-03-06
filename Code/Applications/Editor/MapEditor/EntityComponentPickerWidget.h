#pragma once
#include "QTreeView"
#include "System/Core/Core/IntegralTypes.h"
#include "System/TypeSystem/TypeInfo.h"
#include "QSortFilterProxyModel"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    class KSearchBoxWidget;

    //-------------------------------------------------------------------------

    namespace EntityComponentPicker
    {
        class TreeItem
        {
        public:

            explicit TreeItem( TypeSystem::TypeInfo const* pTypeInfo, QString const& simpleTypeName );
            explicit TreeItem( QString const& name );
            ~TreeItem();

            inline QString const& GetName() const { return m_name; }
            inline bool IsCategory() const { return m_pTypeInfo == nullptr; }
            inline TypeSystem::TypeInfo const* GetTypeInfo() const { KRG_ASSERT( !IsCategory() ); return m_pTypeInfo; }
            inline TreeItem* GetParent() const { return m_pParent; }

            int32 GetNumChildren() const;
            TreeItem* GetChild( int row );
            TreeItem* GetChild( QString const& name ) const;
            int32 GetRowIndex() const;

            void AddChild( TreeItem* pChild );

            QVariant GetDataForColumn( int32 column ) const;
            QIcon GetIcon() const;

        private:

            TypeSystem::TypeInfo const*             m_pTypeInfo = nullptr;
            QString                                 m_name;
            TVector<TreeItem*>                      m_children;
            TreeItem*                               m_pParent = nullptr;
        };

        //-------------------------------------------------------------------------

        class Model final : public QAbstractItemModel
        {
            Q_OBJECT

        public:

            explicit Model( TypeSystem::TypeRegistry const& typeRegistry, QObject* pParent );
            ~Model();

            virtual QVariant data( QModelIndex const& index, int role ) const override;
            virtual Qt::ItemFlags flags( QModelIndex const& index ) const override;
            virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
            virtual QModelIndex index( int row, int column, QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual QModelIndex parent( QModelIndex const& index ) const override;
            virtual int rowCount( QModelIndex const& parentIndex = QModelIndex() ) const override;
            virtual int columnCount( QModelIndex const& parentIndex = QModelIndex() ) const override;

        private:

            TreeItem* FindOrAddCategory( TreeItem* pParentCategory, QString const& categoryName );

        private:

            TreeItem*                               m_pRootItem = nullptr;
        };

        //-------------------------------------------------------------------------

        class FilterModel final : public QSortFilterProxyModel
        {
        public:

            explicit FilterModel( QObject* pParent = nullptr );

            void SetFilterString( QString const& filterString );

        private:

            virtual bool filterAcceptsRow( int32 sourceRow, QModelIndex const& sourceParentIndex ) const override final;
            virtual bool lessThan( QModelIndex const& left, QModelIndex const& right ) const override final;

        private:

            QStringList m_filter;
        };
    }

    //-------------------------------------------------------------------------

    class EntityComponentPickerWidget final : public QWidget
    {
        Q_OBJECT

    signals:

        void OnComponentSelected( TypeSystem::TypeInfo const* pComponentTypeInfo );

    public:

        EntityComponentPickerWidget( TypeSystem::TypeRegistry const& typeRegistry );

    private:

        virtual void focusInEvent( QFocusEvent* pEvent ) override final;

        void ResetPicker();
        void OnSelectionChanged( QModelIndex const& current, QModelIndex const& previous );

    private:

        KSearchBoxWidget*                           m_pFilterBox = nullptr;
        QTreeView*                                  m_pTree = nullptr;
        EntityComponentPicker::Model                m_model;
        EntityComponentPicker::FilterModel          m_filterModel;
    };

}