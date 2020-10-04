#pragma once

#include "System/Core/Types/Containers.h"

#include <QWidget>
#include <QAbstractItemModel>

//-------------------------------------------------------------------------

class QTableView;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class CompilationRequest;

        //-------------------------------------------------------------------------

        class ActiveRequestsViewModel : public QAbstractTableModel
        {
            Q_OBJECT

        public:

            inline ActiveRequestsViewModel() : QAbstractTableModel() {}

            void UpdateRequests( TVector<CompilationRequest const*> const* pPendingRequests, TVector<CompilationRequest const*> const* pActiveRequests );
            inline CompilationRequest const* GetRequest( S32 index ) const { return m_requests[index]; }

        private:

            virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const override final;
            virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const override final;
            virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override final;
            virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override final;

        private:

            TVector<CompilationRequest const*>          m_requests;
        };

        //-------------------------------------------------------------------------

        class ActiveRequestsViewWidget : public QWidget
        {
            Q_OBJECT

        public:

            ActiveRequestsViewWidget();

        public slots:

            void OnRequestsUpdated( TVector<CompilationRequest const*> const* pPendingRequests, TVector<CompilationRequest const*> const* pActiveRequests );

        private:

            QTableView*                                 m_pTableWidget = nullptr;
            ActiveRequestsViewModel*                   m_pModel = nullptr;
        };
    }
}