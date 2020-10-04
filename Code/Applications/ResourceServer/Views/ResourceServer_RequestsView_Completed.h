#pragma once

#include "System/Core/Types/Containers.h"

#include <QWidget>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

//-------------------------------------------------------------------------

class QTableView;
class QLineEdit;
class QPushButton;
class QTextEdit;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class CompilationRequest;

        //-------------------------------------------------------------------------

        class CompletedRequestsViewModel : public QAbstractTableModel
        {
            Q_OBJECT

        public:

            inline CompletedRequestsViewModel() : QAbstractTableModel() {}

            void UpdateRequests( TVector<CompilationRequest const*> const* pRequests, S32 column, Qt::SortOrder order );
            inline CompilationRequest const* GetRequest( S32 index ) const { return m_requests[index]; }

        private:

            virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const override final;
            virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const override final;
            virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override final;
            virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override final;
            virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) override final;

            void SortRequests( int column, Qt::SortOrder order = Qt::AscendingOrder );

        private:

            TVector<CompilationRequest const*>          m_requests;
        };

        //-------------------------------------------------------------------------

        class CompletedRequestsViewWidget : public QWidget
        {
            Q_OBJECT

        signals:

            void RequestClearHistory();

        public:

            CompletedRequestsViewWidget();

        private slots:

            void OnRequestsUpdated( TVector<CompilationRequest const*> const* pRequests );
            void OnSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected );
            void OnCopyArgs();
            void OnExploreToSource();
            void OnExploreToDestination();
            void OnCustomMenuRequested( QPoint pos );

        private:

            QTableView*                                 m_pTableWidget = nullptr;
            QLineEdit*                                  m_pCommandLineWidget = nullptr;
            QPushButton*                                m_pCopyArgsButton = nullptr;
            QPushButton*                                m_pExploreToSourceButton = nullptr;
            QPushButton*                                m_pExploreToDestinationButton = nullptr;
            QTextEdit*                                  m_pLogWidget = nullptr;

            CompletedRequestsViewModel                  m_model;
            CompilationRequest const*                   m_pSelectedRequest = nullptr;
        };
    }
}