#include "ResourceServer_RequestsView_Active.h"
#include "ResourceServer/CompilationRequest.h"
#include "Tools/UI/Widgets/IconWidgets.h"

#include "QTableView"
#include "QHeaderView"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        void ActiveRequestsViewModel::UpdateRequests( TVector<CompilationRequest const*> const* pPendingRequests, TVector<CompilationRequest const*> const* pActiveRequests )
        {
            // Active requests always go first
            m_requests = *pActiveRequests;

            // Then all pending requests
            for ( auto& request : *pPendingRequests )
            {
                m_requests.emplace_back( request );
            }

            emit layoutChanged();
        }

        int ActiveRequestsViewModel::rowCount( const QModelIndex& parent ) const
        {
            return ( int) m_requests.size();
        }

        int ActiveRequestsViewModel::columnCount( const QModelIndex& parent ) const
        {
            return 3;
        }

        QVariant ActiveRequestsViewModel::headerData( int section, Qt::Orientation orientation, int role ) const
        {
            if ( role == Qt::DisplayRole )
            {
                if ( orientation == Qt::Horizontal )
                {
                    if ( section == 0 )
                    {
                        return "";
                    }

                    else if ( section == 1 )
                    {
                        return "Resource";
                    }

                    else if ( section == 2 )
                    {
                        return "Client ID";
                    }
                }
            }

            return QVariant();
        }

        QVariant ActiveRequestsViewModel::data( const QModelIndex& index, int role ) const
        {
            S32 const row = index.row();
            S32 const column = index.column();

            auto pRequest = m_requests[row];

            if ( role == Qt::DisplayRole )
            {
                if ( column == 0 )
                {
                    // Do nothing
                }

                else if ( column == 1 )
                {
                    return pRequest->GetResourceID().c_str();
                }

                else if ( column == 2 )
                {
                    if ( pRequest->IsInternalRequest() )
                    {
                        return "Auto";
                    }
                    else
                    {
                        return pRequest->GetClientID();
                    }
                }
            }

            //-------------------------------------------------------------------------

            if ( role == Qt::DecorationRole )
            {
                if ( column == 0 )
                {
                    if ( pRequest->IsPending() )
                    {
                        return KIcon( MaterialDesign::Alarm_Multiple, QColor( 200, 200, 200 ) );
                    }
                    else if ( pRequest->IsExecuting() )
                    {
                        return KIcon( ForkAwesome::Cogs, QColor( 0, 255, 0 ) );
                    }
                }
            }

            return QVariant();
        }

        //-------------------------------------------------------------------------

        ActiveRequestsViewWidget::ActiveRequestsViewWidget()
            : QWidget()
        {
            m_pModel = new ActiveRequestsViewModel();

            //-------------------------------------------------------------------------

            m_pTableWidget = new QTableView();
            m_pTableWidget->setStyleSheet( "QWidget{ font-size: 11px; }" );
            m_pTableWidget->setModel( m_pModel );
            m_pTableWidget->horizontalHeader()->show();
            m_pTableWidget->horizontalHeader()->setMinimumSectionSize( 6 );
            m_pTableWidget->setSelectionMode( QAbstractItemView::SingleSelection );
            m_pTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
            m_pTableWidget->setTextElideMode( Qt::TextElideMode::ElideMiddle );
            m_pTableWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
            m_pTableWidget->setSortingEnabled( false );

            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Fixed );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Fixed );

            m_pTableWidget->setColumnWidth( 0, 22 );
            m_pTableWidget->setColumnWidth( 2, 150 );

            //-------------------------------------------------------------------------

            auto pViewLayout = new QVBoxLayout( this );
            pViewLayout->setContentsMargins( 2, 0, 4, 0 );
            pViewLayout->setSpacing( 3 );
            pViewLayout->addWidget( m_pTableWidget, 1 );
        }

        void ActiveRequestsViewWidget::OnRequestsUpdated( TVector<CompilationRequest const*> const* pPendingRequests, TVector<CompilationRequest const*> const* pActiveRequests )
        {
            KRG_ASSERT( pPendingRequests != nullptr && pActiveRequests != nullptr );
            m_pModel->UpdateRequests( pPendingRequests, pActiveRequests );
        }
    }
}
