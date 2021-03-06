#include "ResourceServer_MainView.h"
#include "ResourceServer_RequestsView_Active.h"
#include "ResourceServer_RequestsView_Completed.h"
#include "ResourceServer_CompilerView.h"
#include "ResourceServer/ResourceServer.h"
#include "QStackedLayout"
#include "QPushButton"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QGroupBox"
#include "QSplitter"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        MainViewWidget::MainViewWidget( ResourceServer& resourceServer )
            : QWidget()
        {
            setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

            // SideBar
            //-------------------------------------------------------------------------

            auto pRequestViewButton = new QPushButton( KIcon( ForkAwesome::Tasks ), "" );
            pRequestViewButton->setIconSize( QSize( 24, 24 ) );
            pRequestViewButton->setToolTip( "Compilation Requests" );
            connect( pRequestViewButton, &QPushButton::clicked, this, [this] () { emit ChangePageIndex( 0 ); } );

            auto pInfoViewButton = new QPushButton( KIcon( ForkAwesome::Cog ), "" );
            pInfoViewButton->setIconSize( QSize( 24, 24 ) );
            pInfoViewButton->setToolTip( "Compilers" );
            connect( pInfoViewButton, &QPushButton::clicked, this, [this] () { emit ChangePageIndex( 1 ); } );

            auto pSidebar = new QWidget;
            pSidebar->setStyleSheet( "QWidget{ background-color: #404040; }" );

            auto pSidebarLayout = new QVBoxLayout( pSidebar );
            pSidebarLayout->setMargin( 4 );
            pSidebarLayout->addWidget( pRequestViewButton );
            pSidebarLayout->addWidget( pInfoViewButton );
            pSidebarLayout->addStretch( 1 );

            // Create Pages
            //-------------------------------------------------------------------------

            auto pStackWidget = new QWidget;
            m_pStackedLayout = new QStackedLayout( pStackWidget );
            m_pStackedLayout->addWidget( CreateRequestsPage( resourceServer ) );
            m_pStackedLayout->addWidget( CreateCompilerPage( resourceServer ) );

            //-------------------------------------------------------------------------

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( pSidebar, 0 );
            pMainLayout->addWidget( pStackWidget, 1 );

            //-------------------------------------------------------------------------

            connect( this, &MainViewWidget::ChangePageIndex, this, &MainViewWidget::OnChangePageIndex );
        }

        void MainViewWidget::OnChangePageIndex( int32 pageIdx )
        {
            KRG_ASSERT( m_pStackedLayout != nullptr && pageIdx < m_pStackedLayout->count() );
            m_pStackedLayout->setCurrentIndex( pageIdx );
        }

        QWidget* MainViewWidget::CreateCompilerPage( ResourceServer& resourceServer )
        {
            return new CompilerView( resourceServer );
        }

        QWidget* MainViewWidget::CreateRequestsPage( ResourceServer& resourceServer )
        {
            auto pActiveRequestsWidget = new ActiveRequestsViewWidget();
            auto pCompletedRequestsWidget = new CompletedRequestsViewWidget();

            connect( &resourceServer, SIGNAL( ActiveRequestsUpdated( TVector<CompilationRequest const*> const*, TVector<CompilationRequest const*> const* ) ), pActiveRequestsWidget, SLOT( OnRequestsUpdated( TVector<CompilationRequest const*> const*, TVector<CompilationRequest const*> const* ) ) );
            connect( &resourceServer, SIGNAL( CompletedRequestsUpdated( TVector<CompilationRequest const*> const* ) ), pCompletedRequestsWidget, SLOT( OnRequestsUpdated( TVector<CompilationRequest const*> const* ) ) );
            connect( pCompletedRequestsWidget, SIGNAL( RequestClearHistory() ), &resourceServer, SLOT( OnCleanupOldRequests() ) );

            //-------------------------------------------------------------------------
            // Group boxes
            //-------------------------------------------------------------------------

            auto pActiveGroupBox = new QGroupBox( "Active and Pending Requests" );
            auto pActiveBoxLayout = new QHBoxLayout( pActiveGroupBox );
            pActiveBoxLayout->setMargin( 0 );
            pActiveBoxLayout->addWidget( pActiveRequestsWidget, 1 );

            auto pCompletedGroupBox = new QGroupBox( "Completed Requests" );
            auto pCompletedBoxLayout = new QHBoxLayout( pCompletedGroupBox );
            pCompletedBoxLayout->setMargin( 0 );
            pCompletedBoxLayout->addWidget( pCompletedRequestsWidget, 1 );

            //-------------------------------------------------------------------------
            // Layout
            //-------------------------------------------------------------------------

            auto pSplitter = new QSplitter();
            pSplitter->setOrientation( Qt::Vertical );
            pSplitter->setChildrenCollapsible( false );
            pSplitter->setMinimumHeight( 100 );
            pSplitter->addWidget( pActiveGroupBox );
            pSplitter->addWidget( pCompletedGroupBox );

            auto pRequestPageWidget = new QWidget;
            auto pViewLayout = new QVBoxLayout( pRequestPageWidget );
            pViewLayout->setMargin( 3 );
            pViewLayout->addWidget( pSplitter, 1 );

            //-------------------------------------------------------------------------

            // This needs to be done after the setLayout so that the SizeHint will return the calculated size.
            QList<int> sizes;
            sizes.append( 0.1 * sizeHint().height() );
            sizes.append( 0.9 * sizeHint().height() );
            pSplitter->setSizes( sizes );

            return pRequestPageWidget;
        }
    }
}