#include "ResourceServerApplication.h"
#include "ResourceServer/ResourceServer.h"
#include "Views/ResourceServer_MainView.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/LinkButtonWidget.h"

#include <QMainWindow>
#include <QAction>
#include <QIcon>
#include <QToolBar>
#include <QPushButton>
#include <QProcess>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        void ResourceServerMainWindow::closeEvent( QCloseEvent* pEvent )
        {
            Window::closeEvent( pEvent );
            HideWindow();
            pEvent->ignore();
        }

        //-------------------------------------------------------------------------

        ResourceServerApplication::ResourceServerApplication( int argc, char* argv[] )
            : Tools::Application( argc, argv )
            , m_instanceLock( "ResourceServer" )
        {
            m_applicationName = "Kruger Resource Server";
            m_applicationIconPath = ":/KRG/ResourceServer.png";
            m_applicationVersion = "0.0";

            // Tray Icon
            //-------------------------------------------------------------------------

            m_pQuitAction = new QAction( "&Quit", this );
            connect( m_pQuitAction, &QAction::triggered, qApp, &QCoreApplication::quit );

            m_pTrayIconMenu = new QMenu();
            m_pTrayIconMenu->addAction( m_pQuitAction );

            m_pTrayIcon = new QSystemTrayIcon( this );
            m_pTrayIcon->setIcon( m_idleIcon );
            m_pTrayIcon->setToolTip( m_applicationName );
            m_pTrayIcon->setContextMenu( m_pTrayIconMenu );

            connect( m_pServer, SIGNAL( BusyStateChanged() ), this, SLOT( OnServerBusyStateChanged() ) );
            connect( m_pTrayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( OnTrayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
            connect( qApp, &QCoreApplication::aboutToQuit, m_pTrayIcon, &QSystemTrayIcon::hide );
            m_pTrayIcon->show();
        }

        bool ResourceServerApplication::Initialize()
        {
            // Try get mutex lock as only a single instance of the resource server is allowed
            if ( !m_instanceLock.TryLock() )
            {
                return FatalError( QString( "Only a single instance of the resource server is allowed!" ) );
            }

            // Create resource server
            //-------------------------------------------------------------------------

            m_pServer = KRG::New<ResourceServer>();
            if ( !m_pServer->Initialize() )
            {
                return FatalError( QString( "Failed to initialize server: " ) + m_pServer->GetErrorMessage().c_str() );
            }

            // Create network server thread
            //-------------------------------------------------------------------------

            m_pServerThread = KRG::New<QThread>();
            m_pServer->moveToThread( m_pServerThread );

            connect( m_pServerThread, SIGNAL( started() ), m_pServer, SLOT( OnThreadStarted() ) );
            connect( m_pServerThread, SIGNAL( finished() ), m_pServer, SLOT( OnThreadStopped() ) );

            m_pServerThread->start();

            // Create UI
            //-------------------------------------------------------------------------

            CreateMainContent();
            CreateStatusBarContent();

            return true;
        }

        void ResourceServerApplication::Shutdown()
        {
            if ( m_pServerThread != nullptr )
            {
                m_pServerThread->quit();
                m_pServerThread->wait();
                KRG::Delete( m_pServerThread );
            }

            //-------------------------------------------------------------------------

            m_pServer->Shutdown();
            KRG::Delete( m_pServer );
        }

        void ResourceServerApplication::OnTrayIconActivated( QSystemTrayIcon::ActivationReason reason )
        {
            switch ( reason )
            {
                case QSystemTrayIcon::DoubleClick:
                {
                    if ( !m_pWindow->isVisible() )
                    {
                        m_pWindow->ShowWindow();
                        m_pWindow->activateWindow();
                    }
                    else // Restore the window
                    {
                        m_pWindow->setWindowState( m_pWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive );
                    }
                }
                break;
            }
        }

        void ResourceServerApplication::OnServerBusyStateChanged()
        {
            if ( m_pServer->IsBusy() )
            {
                m_pTrayIcon->setIcon( m_busyIcon );
            }
            else
            {
                m_pTrayIcon->setIcon( m_idleIcon );
            }
        }

        void ResourceServerApplication::CreateMainContent()
        {
            auto pMainContent = m_pWindow->GetMainContentWidget();
            auto pMainLayout = new QVBoxLayout( pMainContent );
            pMainLayout->setContentsMargins( 0, 0, 0, 0 );
            pMainLayout->addWidget( new MainViewWidget( *m_pServer ) );
        }

        void ResourceServerApplication::CreateStatusBarContent()
        {
            auto pStatusBarWidget = new QWidget;
            auto pStatusBarLayout = new QHBoxLayout( pStatusBarWidget );
            pStatusBarLayout->setAlignment( Qt::AlignCenter );
            pStatusBarLayout->setSpacing( 4 );
            pStatusBarLayout->setContentsMargins( 8, 0, 0, 2 );

            // Data Folder
            //-------------------------------------------------------------------------

            pStatusBarLayout->addWidget( new KIconLabel( MaterialDesign::Folder_Open ) );
            pStatusBarLayout->addWidget( new QLabel( "Source Data: " ) );

            auto pDataDirectoryButton = new KLinkButton( m_pServer->GetSourceDataDir().c_str() );
            pDataDirectoryButton->setIconSize( QSize( 24, 24 ) );

            auto openInExplorer = [this] ()
            {
                QProcess* pProcess = new QProcess( this );
                QStringList args( m_pServer->GetSourceDataDir().c_str() );
                pProcess->start( "explorer.exe", args );
            };

            connect( pDataDirectoryButton, &QPushButton::clicked, this, openInExplorer );
            pStatusBarLayout->addWidget( pDataDirectoryButton );

            //-------------------------------------------------------------------------

            pStatusBarLayout->addStretch( 1 );

            // Server Address Widget
            //-------------------------------------------------------------------------

            pStatusBarLayout->addWidget( new KIconLabel( MaterialDesign::Lan ) );

            auto pNetworkAddressLabel = new QLabel( "Address: " );
            pStatusBarLayout->addWidget( pNetworkAddressLabel );

            QLabel* serverAddressLabel = new QLabel();
            serverAddressLabel->setText( m_pServer->GetNetworkAddress().c_str() );
            pStatusBarLayout->addWidget( serverAddressLabel );

            //-------------------------------------------------------------------------

            auto pStatusBar = m_pWindow->GetStatusBar();
            pStatusBar->addWidget( pStatusBarWidget, 1 );
            pStatusBar->setVisible( true );
        }

        void ResourceServerApplication::OnServerStopped()
        {}

        Tools::Window* ResourceServerApplication::CreateMainWindow( QString const& windowName, QString const& windowIconPath, QString const& windowTitle )
        {
            return new ResourceServerMainWindow( windowName, windowIconPath, windowTitle );
        }
    }
}