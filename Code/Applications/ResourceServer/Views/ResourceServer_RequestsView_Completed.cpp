#include "ResourceServer_RequestsView_Completed.h"
#include "ResourceServer/CompilationRequest.h"
#include "Tools/UI/Widgets/IconWidgets.h"

#include "QBoxLayout"
#include "QPushButton"
#include "QTableView"
#include "QTextEdit"
#include "QLineEdit"
#include "QClipboard"
#include "QApplication"
#include "QProcess"
#include "QDir"
#include "QHeaderView"
#include "QDateTime"
#include "QMenu"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        void CompletedRequestsViewModel::UpdateRequests( TVector<CompilationRequest const*> const* pRequests, S32 column, Qt::SortOrder order )
        {
            emit layoutAboutToBeChanged();
            m_requests = *pRequests;
            SortRequests( column, order );
            emit layoutChanged();
        }

        int CompletedRequestsViewModel::rowCount( const QModelIndex& parent ) const
        {
            return (int) m_requests.size();
        }

        int CompletedRequestsViewModel::columnCount( const QModelIndex& parent ) const
        {
            return 6;
        }

        QVariant CompletedRequestsViewModel::headerData( int section, Qt::Orientation orientation, int role ) const
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
                        return "Requested";
                    }

                    else if ( section == 2 )
                    {
                        return "Source";
                    }

                    else if ( section == 3 )
                    {
                        return "Destination";
                    }

                    else if ( section == 4 )
                    {
                        return "UpToDate";
                    }

                    else if ( section == 5 )
                    {
                        return "Compile";
                    }
                }
            }
            
            if( role == Qt::ToolTipRole )
            {
                if ( section == 1 )
                {
                    return "Time Requested";
                }

                else if ( section == 2 )
                {
                    return "Source Path";
                }

                else if ( section == 3 )
                {
                    return "Destination Path";
                }

                else if ( section == 4 )
                {
                    return "Time taken to perform the up-to-date check";
                }

                else if ( section == 5 )
                {
                    return "Time taken to compile the resource";
                }
            }

            return QVariant();
        }

        QVariant CompletedRequestsViewModel::data( const QModelIndex& index, int role ) const
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

                if ( column == 1 )
                {
                    return pRequest->GetTimeRequested().toString( "h:mm:ss" );
                }

                if ( column == 2 )
                {
                    return pRequest->GetSourceFilePath().c_str();
                }

                else if ( column == 3 )
                {
                    return pRequest->GetDestinationFilePath().c_str();
                }

                else if ( column == 4 )
                {
                    return QString( "%1s" ).arg( pRequest->GetUptoDateCheckElapsedTime().ToFloat(), 0, 'f', 3 );
                }

                else if ( column == 5 )
                {
                    return QString( "%1s" ).arg( pRequest->GetCompilationElapsedTime().ToFloat(), 0, 'f', 3 );
                }
            }

            //-------------------------------------------------------------------------

            if ( role == Qt::DecorationRole )
            {
                if ( column == 0 )
                {
                    if ( pRequest->HasFailed() )
                    {
                        return KIcon( ForkAwesome::Times, QColor( 255, 0, 0 ) );
                    }
                    else if ( pRequest->HasSucceededWithWarnings() )
                    {
                        return KIcon( ForkAwesome::Exclamation_Triangle, QColor( 255, 255, 0 ) );
                    }
                    else
                    {
                        return KIcon( ForkAwesome::Check, QColor( 0, 255, 0 ) );
                    }
                }
            }

            return QVariant();
        }

        void CompletedRequestsViewModel::sort( int column, Qt::SortOrder order )
        {
            emit layoutAboutToBeChanged();
            SortRequests( column, order );
            emit layoutChanged();
        }

        void CompletedRequestsViewModel::SortRequests( int column, Qt::SortOrder order )
        {
            if ( column == 0 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    S32 const a = ( S32 ) pA->GetStatus();
                    S32 const b = ( S32 ) pB->GetStatus();

                    if ( a == b )
                    {
                        return pA->GetResourceID().ToString() < pB->GetResourceID().ToString();
                    }

                    if( order == Qt::AscendingOrder)
                    {
                        return a < b;
                    }
                    else
                    {
                        return a > b;
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }

            else if ( column == 1 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    auto const& a = pA->GetTimeRequested();
                    auto const& b = pB->GetTimeRequested();

                    if ( a == b )
                    {
                        return pA->GetResourceID().ToString() < pB->GetResourceID().ToString();
                    }

                    if ( order == Qt::AscendingOrder )
                    {
                        return a < b;
                    }
                    else
                    {
                        return a > b;
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }

            else if ( column == 2 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    if ( order == Qt::AscendingOrder )
                    {
                        return pA->GetSourceFilePath().ToString() < pB->GetSourceFilePath().ToString();
                    }
                    else
                    {
                        return pA->GetSourceFilePath().ToString() > pB->GetSourceFilePath().ToString();
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }

            else if ( column == 3 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    if ( order == Qt::AscendingOrder )
                    {
                        return pA->GetDestinationFilePath().ToString() < pB->GetDestinationFilePath().ToString();
                    }
                    else
                    {
                        return pA->GetDestinationFilePath().ToString() > pB->GetDestinationFilePath().ToString();
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }

            else if ( column == 4 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    F32 const a = pA->GetUptoDateCheckElapsedTime().ToFloat();
                    F32 const b = pB->GetUptoDateCheckElapsedTime().ToFloat();

                    if ( a == b )
                    {
                        return pA->GetResourceID().ToString() < pB->GetResourceID().ToString();
                    }

                    if ( order == Qt::AscendingOrder )
                    {
                        return a < b;
                    }
                    else
                    {
                        return a > b;
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }

            else if ( column == 5 )
            {
                auto sortPredicate = [order] ( CompilationRequest const* const& pA, CompilationRequest const* const& pB )
                {
                    F32 const a = pA->GetCompilationElapsedTime().ToFloat();
                    F32 const b = pB->GetCompilationElapsedTime().ToFloat();

                    if ( a == b )
                    {
                        return pA->GetResourceID().ToString() < pB->GetResourceID().ToString();
                    }

                    if ( order == Qt::AscendingOrder )
                    {
                        return a < b;
                    }
                    else
                    {
                        return a > b;
                    }
                };

                eastl::sort( m_requests.begin(), m_requests.end(), sortPredicate );
            }
        }

        //-------------------------------------------------------------------------

        CompletedRequestsViewWidget::CompletedRequestsViewWidget()
            : QWidget()
        {
            //-------------------------------------------------------------------------
            // Table
            //-------------------------------------------------------------------------

            m_pTableWidget = new QTableView;
            m_pTableWidget->setStyleSheet( "QWidget{ font-size: 11px; }" );
            m_pTableWidget->setModel( &m_model );
            m_pTableWidget->setSelectionMode( QAbstractItemView::SelectionMode::SingleSelection );
            m_pTableWidget->setSelectionBehavior( QAbstractItemView::SelectionBehavior::SelectRows );
            m_pTableWidget->setSortingEnabled( true );
            m_pTableWidget->horizontalHeader()->show();
            m_pTableWidget->horizontalHeader()->setMinimumSectionSize( 6 );
            m_pTableWidget->setShowGrid( true );
            m_pTableWidget->setWordWrap( false );
            m_pTableWidget->setTextElideMode( Qt::TextElideMode::ElideMiddle );
            m_pTableWidget->setContextMenuPolicy( Qt::CustomContextMenu );

            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Fixed );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Fixed );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Stretch );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 4, QHeaderView::Fixed );
            m_pTableWidget->horizontalHeader()->setSectionResizeMode( 5, QHeaderView::Fixed );

            m_pTableWidget->horizontalHeader()->setSortIndicator( 1, Qt::DescendingOrder );

            m_pTableWidget->setColumnWidth( 0, 22 );
            m_pTableWidget->setColumnWidth( 1, 75 );
            m_pTableWidget->setColumnWidth( 4, 75 );
            m_pTableWidget->setColumnWidth( 5, 75 );

            connect( m_pTableWidget, SIGNAL( customContextMenuRequested( QPoint ) ), SLOT( OnCustomMenuRequested( QPoint ) ) );

            connect
            (
                m_pTableWidget->selectionModel(),
                SIGNAL( selectionChanged( QItemSelection const&, QItemSelection const& ) ),
                this,
                SLOT( OnSelectionChanged( QItemSelection const&, QItemSelection const& ) )
            );

            //-------------------------------------------------------------------------
            // Controls
            //-------------------------------------------------------------------------

            m_pCommandLineWidget = new QLineEdit;
            m_pCommandLineWidget->setReadOnly( true );

            m_pCopyArgsButton = new QPushButton( KIcon( MaterialDesign::Content_Copy ), "Copy Args" );
            m_pCopyArgsButton->setFixedWidth( 130 );
            m_pCopyArgsButton->setStyleSheet( "QPushButton{ qproperty-iconSize: 16px; }" );
            connect( m_pCopyArgsButton, &QPushButton::clicked, this, &CompletedRequestsViewWidget::OnCopyArgs );

            m_pExploreToSourceButton = new QPushButton( KIcon( ForkAwesome::File ), "Go To Source" );
            m_pExploreToSourceButton->setFixedWidth( 130 );
            m_pExploreToSourceButton->setStyleSheet( "QPushButton{ qproperty-iconSize: 16px; }" );
            m_pExploreToSourceButton->setDisabled( true );
            connect( m_pExploreToSourceButton, &QPushButton::clicked, this, &CompletedRequestsViewWidget::OnExploreToSource );

            m_pExploreToDestinationButton = new QPushButton( KIcon( ForkAwesome::Cube, QColor( 0, 255, 0 ) ), "Go To Destination" );
            m_pExploreToDestinationButton->setFixedWidth( 130 );
            m_pExploreToDestinationButton->setStyleSheet( "QPushButton{  qproperty-iconSize: 16px; }" );
            m_pExploreToDestinationButton->setDisabled( true );
            connect( m_pExploreToDestinationButton, &QPushButton::clicked, this, &CompletedRequestsViewWidget::OnExploreToDestination );

            auto pArgsLayout = new QHBoxLayout;
            pArgsLayout->setSpacing( 4 );
            pArgsLayout->addWidget( m_pCommandLineWidget, 1 );
            pArgsLayout->addWidget( m_pCopyArgsButton, 0 );
            pArgsLayout->addWidget( m_pExploreToSourceButton, 0 );
            pArgsLayout->addWidget( m_pExploreToDestinationButton, 0 );

            //-------------------------------------------------------------------------
            // Log
            //-------------------------------------------------------------------------

            m_pLogWidget = new QTextEdit;
            m_pLogWidget->setStyleSheet( "QTextEdit{ font-size: 10px; }" );
            m_pLogWidget->setReadOnly( true );
            m_pLogWidget->setFixedHeight( 100 );

            //-------------------------------------------------------------------------
            // Layout
            //-------------------------------------------------------------------------

            auto pViewLayout = new QVBoxLayout( this );
            pViewLayout->setContentsMargins( 2, 0, 4, 0 );
            pViewLayout->setSpacing( 3 );

            pViewLayout->addWidget( m_pTableWidget, 1 );
            pViewLayout->addLayout( pArgsLayout, 0 );
            pViewLayout->addWidget( m_pLogWidget, 0 );
        }

        void CompletedRequestsViewWidget::OnRequestsUpdated( TVector<CompilationRequest const*> const* pRequests )
        {
            KRG_ASSERT( pRequests != nullptr );

            S32 const sortedColumnIdx = m_pTableWidget->horizontalHeader()->sortIndicatorSection();
            Qt::SortOrder const sortOrder = m_pTableWidget->horizontalHeader()->sortIndicatorOrder();

            m_model.UpdateRequests( pRequests, sortedColumnIdx, sortOrder );
            m_pTableWidget->clearSelection();
        }

        void CompletedRequestsViewWidget::OnSelectionChanged( QItemSelection const& selected, QItemSelection const& deselected )
        {
            QModelIndexList indices = selected.indexes();

            if ( indices.count() != 0 )
            {
                QModelIndex index = indices[0];
                S32 const requestIdx = index.row();
                m_pSelectedRequest = m_model.GetRequest( requestIdx );

                m_pCommandLineWidget->setText( QString( "-compile %1" ).arg( m_pSelectedRequest->GetCompilerArgs() ) );
                m_pLogWidget->setText( m_pSelectedRequest->GetLog() );

                m_pExploreToSourceButton->setDisabled( false );
                m_pExploreToDestinationButton->setDisabled( false );
            }
            else
            {
                m_pSelectedRequest = nullptr;

                m_pCommandLineWidget->setText( "" );
                m_pLogWidget->setText( "" );

                m_pExploreToSourceButton->setDisabled( true );
                m_pExploreToDestinationButton->setDisabled( true );
            }
        }

        void CompletedRequestsViewWidget::OnCopyArgs()
        {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText( m_pCommandLineWidget->text() );
        }

        void CompletedRequestsViewWidget::OnExploreToSource()
        {
            QProcess* pProcess = new QProcess( this );
            QStringList args;
            args << "/select," << QDir::toNativeSeparators( m_pSelectedRequest->GetSourceFilePath().c_str() );
            pProcess->start( "explorer.exe", args );
        }

        void CompletedRequestsViewWidget::OnExploreToDestination()
        {
            QProcess* pProcess = new QProcess( this );
            QStringList args;
            args << "/select," << QDir::toNativeSeparators( m_pSelectedRequest->GetDestinationFilePath().c_str() );
            pProcess->start( "explorer.exe", args );
        }

        void CompletedRequestsViewWidget::OnCustomMenuRequested( QPoint pos )
        {
            auto pClearAction = new QAction( "Clear Request History", this );
            connect( pClearAction, &QAction::triggered, this, [this] () { emit RequestClearHistory(); } );

            QMenu* menu = new QMenu( this );
            menu->addAction( pClearAction );
            menu->popup( m_pTableWidget->viewport()->mapToGlobal( pos ) );
        }
    }
}
