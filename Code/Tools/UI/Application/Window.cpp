#include "Window.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Style/DarkStyle.h"
#include "System/Core/FileSystem/FileSystem.h"

#include <windowsx.h>

#pragma comment(lib, "dwmapi.lib")

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        Window::Window( QString const& windowName, QString const& windowIconPath, QString const& windowTitle )
            : QMainWindow()
            , m_windowName( windowName )
        {
            setWindowFlags( Qt::FramelessWindowHint );
            setWindowIcon( QIcon( windowIconPath ) );

            //-------------------------------------------------------------------------

            // Add the window title bar in the first layout in vertical.
            m_pTitleBar = CreateTitleBar( windowIconPath, windowTitle );
            KRG_ASSERT( m_pTitleBar != nullptr );

            // Create and add the main window content here
            m_pMainContent = new QWidget();

            // Create status bar (hide by default)
            m_pStatusBar = new QStatusBar();
            m_pStatusBar->setVisible( false );

            //-------------------------------------------------------------------------

            auto pCentralWidget = new QWidget( this );
            pCentralWidget->setObjectName( "WindowCentralWidget" );
            pCentralWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

            auto pVerticalLayout = new QVBoxLayout( pCentralWidget );
            pVerticalLayout->setAlignment( Qt::AlignTop );
            pVerticalLayout->setSpacing( 0 );
            pVerticalLayout->setContentsMargins( 0, 0, 0, 0 );

            pVerticalLayout->addWidget( m_pTitleBar, 0, Qt::AlignTop );
            pVerticalLayout->addWidget( m_pMainContent, 1 );
            pVerticalLayout->addWidget( m_pStatusBar, 0 );

            setCentralWidget( pCentralWidget );
        }

        //-------------------------------------------------------------------------

        void Window::ShowWindow()
        {
            FileSystemPath const settingsIniPath = FileSystem::GetCurrentProcessPath() + m_windowName.toUtf8().constData() + ".ini";
            QSettings settings( settingsIniPath.c_str(), QSettings::IniFormat );
            restoreGeometry( settings.value( "Window/Geometry" ).toByteArray() );
            restoreState( settings.value( "Window/WindowState" ).toByteArray() );

            //-------------------------------------------------------------------------

            show();

            settings.beginGroup( "Window/WindowState" );
            if ( settings.value( "maximized", isMaximized() ).toBool() )
            {
                showNormal();
                showMaximized();
            }
            else
            {
                showNormal();
            }
            settings.endGroup();
        }

        //-------------------------------------------------------------------------

        QWidget* Window::CreateTitleBar( QString const& windowIconPath, QString const& windowTitle )
        {
            auto pTitleBar = new QWidget();
            pTitleBar->setFixedHeight( 30 );
            pTitleBar->setObjectName( "WindowTitleBar" );
            pTitleBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

            auto pHorizontalLayout = new QHBoxLayout( pTitleBar );
            pHorizontalLayout->setMargin( 0 );
            pHorizontalLayout->setSpacing( 0 );

            //-------------------------------------------------------------------------
            // Title bar icon and title
            //-------------------------------------------------------------------------

            m_pTitleBarIcon = new KIconLabel( QIcon( windowIconPath ), 16, 16 );
            m_pTitleBarIcon->setObjectName( "WindowTitleBarIcon" );
            m_pTitleBarIcon->setFocusPolicy( Qt::NoFocus );
            m_pTitleBarIcon->setToolTip( windowTitle );

            pHorizontalLayout->addWidget( m_pTitleBarIcon, 0 );

            m_pTitleBarLabel = new QLabel();
            m_pTitleBarLabel->setObjectName( "WindowTitleBarLabel" );
            m_pTitleBarLabel->setText( windowTitle );

            pHorizontalLayout->addWidget( m_pTitleBarLabel, 0 );

            //-------------------------------------------------------------------------
            // Title bar content
            //-------------------------------------------------------------------------

            // Add left title bar content
            m_pTitleBarContentLeft = new QWidget;
            pHorizontalLayout->addWidget( m_pTitleBarContentLeft );

            // Empty Space
            pHorizontalLayout->addWidget( new QWidget( m_pTitleBar ), 1 );

            // Add right title bar content
            m_pTitleBarContentRight = new QWidget;
            pHorizontalLayout->addWidget( m_pTitleBarContentRight );

            //-------------------------------------------------------------------------
            // Debug style reload button
            //-------------------------------------------------------------------------

            auto pReloadStylesButton = new QPushButton( KIcon( MaterialDesign::Refresh ), "" );
            pReloadStylesButton->setIconSize( QSize( 16, 16 ) );

            auto reloadStyles = [this] ()
            {
                Tools::Style::InitializeDarkStyle( true );
            };

            connect( pReloadStylesButton, &QPushButton::clicked, this, reloadStyles );

            pHorizontalLayout->addWidget( pReloadStylesButton );

            //-------------------------------------------------------------------------
            // Title bar controls
            //-------------------------------------------------------------------------

            m_pMinimizeButton = new QPushButton();
            m_pMinimizeButton->setFixedSize( 36, 29 );
            m_pMinimizeButton->setObjectName( "MinimizeButton" );
            m_pMinimizeButton->setToolTip( tr( "Minimize" ) );
            m_pMinimizeButton->setFocusPolicy( Qt::NoFocus );
            connect( m_pMinimizeButton, SIGNAL( clicked() ), this, SLOT( OnMinimizeClicked() ) );

            pHorizontalLayout->addWidget( m_pMinimizeButton );

            //-------------------------------------------------------------------------

            m_pMaximizeButton = new QPushButton();
            m_pMaximizeButton->setFixedSize( 36, 29 );
            m_pMaximizeButton->setObjectName( "MaximizeButton" );
            m_pMaximizeButton->setToolTip( tr( "Maximize" ) );
            m_pMaximizeButton->setVisible( true );
            m_pMaximizeButton->setFocusPolicy( Qt::NoFocus );
            connect( m_pMaximizeButton, SIGNAL( clicked() ), this, SLOT( OnMaximizeClicked() ) );

            pHorizontalLayout->addWidget( m_pMaximizeButton );

            //-------------------------------------------------------------------------

            m_pRestoreButton = new QPushButton();
            m_pRestoreButton->setFixedSize( 36, 29 );
            m_pRestoreButton->setObjectName( "RestoreButton" );
            m_pRestoreButton->setToolTip( tr( "Restore" ) );
            m_pRestoreButton->setVisible( false );
            m_pRestoreButton->setFocusPolicy( Qt::NoFocus );
            connect( m_pRestoreButton, SIGNAL( clicked() ), this, SLOT( OnRestoreClicked() ) );

            pHorizontalLayout->addWidget( m_pRestoreButton );

            //-------------------------------------------------------------------------

            m_pCloseButton = new QPushButton();
            m_pCloseButton->setFixedSize( 36, 29 );
            m_pCloseButton->setObjectName( "CloseButton" );
            m_pCloseButton->setToolTip( tr( "Close" ) );
            m_pCloseButton->setFocusPolicy( Qt::NoFocus );
            connect( m_pCloseButton, SIGNAL( clicked() ), this, SLOT( OnCloseClicked() ) );

            pHorizontalLayout->addWidget( m_pCloseButton );

            return pTitleBar;
        }

        //-------------------------------------------------------------------------

        void Window::closeEvent( QCloseEvent* pEvent )
        {
            FileSystemPath const settingsIniPath = FileSystem::GetCurrentProcessPath() + m_windowName.toUtf8().constData() + ".ini";
            QSettings settings( settingsIniPath.c_str(), QSettings::IniFormat );
            settings.setValue( "Window/WindowState", saveState() );
            settings.setValue( "Window/Geometry", saveGeometry() );

            //-------------------------------------------------------------------------

            pEvent->accept();
        }

        bool Window::nativeEvent( QByteArray const& eventType, void* pMessageData, long* pResult )
        {
            Q_UNUSED( eventType );

            auto msg = static_cast<MSG*>( pMessageData );
            switch ( msg->message )
            {
                case WM_NCHITTEST:
                {
                    // Clear result
                    *pResult = 0;

                    // If we are not maximized check if we should show the resize icons
                    if( !windowState().testFlag( Qt::WindowMaximized ) )
                    {
                        static LONG const borderHitTestWidth = 8; // in pixels
                        RECT winrect;
                        GetWindowRect( reinterpret_cast<HWND>( winId() ), &winrect );

                        long const x = GET_X_LPARAM( msg->lParam );
                        long const y = GET_Y_LPARAM( msg->lParam );

                        bool const canResizeWidth = minimumWidth() != maximumWidth();
                        if ( canResizeWidth )
                        {
                            //left border
                            if ( x >= winrect.left && x < winrect.left + borderHitTestWidth )
                            {
                                *pResult = HTLEFT;
                            }
                            //right border
                            if ( x < winrect.right && x >= winrect.right - borderHitTestWidth )
                            {
                                *pResult = HTRIGHT;
                            }
                        }

                        bool const canResizeHeight = minimumHeight() != maximumHeight();
                        if ( canResizeHeight )
                        {
                            //bottom border
                            if ( y < winrect.bottom && y >= winrect.bottom - borderHitTestWidth )
                            {
                                *pResult = HTBOTTOM;
                            }
                            //top border
                            if ( y >= winrect.top && y < winrect.top + borderHitTestWidth )
                            {
                                *pResult = HTTOP;
                            }
                        }

                        if ( canResizeWidth && canResizeHeight )
                        {
                            //bottom left corner
                            if ( x >= winrect.left && x < winrect.left + borderHitTestWidth &&
                                 y < winrect.bottom && y >= winrect.bottom - borderHitTestWidth )
                            {
                                *pResult = HTBOTTOMLEFT;
                            }
                            //bottom right corner
                            if ( x < winrect.right && x >= winrect.right - borderHitTestWidth &&
                                 y < winrect.bottom && y >= winrect.bottom - borderHitTestWidth )
                            {
                                *pResult = HTBOTTOMRIGHT;
                            }
                            //top left corner
                            if ( x >= winrect.left && x < winrect.left + borderHitTestWidth &&
                                 y >= winrect.top && y < winrect.top + borderHitTestWidth )
                            {
                                *pResult = HTTOPLEFT;
                            }
                            //top right corner
                            if ( x < winrect.right && x >= winrect.right - borderHitTestWidth &&
                                 y >= winrect.top && y < winrect.top + borderHitTestWidth )
                            {
                                *pResult = HTTOPRIGHT;
                            }
                        }

                        // If the hit result succeeded return it
                        if ( *pResult != 0 )
                        {
                            return true;
                        }
                    }

                    //-------------------------------------------------------------------------

                    // Check if we are over the title bar
                    auto const cursorPos = QCursor::pos();
                    auto const pWidgetUnderCursor = QApplication::widgetAt( cursorPos );

                    if ( pWidgetUnderCursor != nullptr )
                    {
                        auto isChildOf = [] ( QWidget* pWidget, QWidget* pDesiredParent )
                        {
                            auto pParentWidget = pWidget->parentWidget();
                            while ( pParentWidget != nullptr )
                            {
                                if ( pParentWidget == pDesiredParent )
                                {
                                    return true;
                                }

                                pParentWidget = pParentWidget->parentWidget();
                            }

                            return false;
                        };

                        // Identify the title bar area
                        bool const isChildOfTitleBar = isChildOf( pWidgetUnderCursor, m_pTitleBar );
                        if ( isChildOfTitleBar )
                        {
                            bool const isButtonWidget = qobject_cast<QPushButton*>( pWidgetUnderCursor ) != nullptr;
                            if ( !isButtonWidget && !isChildOf( pWidgetUnderCursor, m_pTitleBarContentLeft ) && !isChildOf( pWidgetUnderCursor, m_pTitleBarContentRight ) )
                            {
                                *pResult = HTCAPTION;
                                return true;
                            }
                        }
                    }
                }
                break;
            }

            return QMainWindow::nativeEvent( eventType, pMessageData, pResult );
        }

        void Window::changeEvent( QEvent* pEvent )
        {
            QMainWindow::changeEvent( pEvent );

            if ( pEvent->type() == QEvent::WindowStateChange )
            {
                auto ev = static_cast<QWindowStateChangeEvent*>( pEvent );
                if ( !( ev->oldState() & Qt::WindowMaximized ) && windowState() & Qt::WindowMaximized )
                {
                    m_pRestoreButton->setVisible( true );
                    m_pMaximizeButton->setVisible( false );
                }
                else
                {
                    m_pRestoreButton->setVisible( false );
                    m_pMaximizeButton->setVisible( true );
                }
            }
        }

        void Window::OnMinimizeClicked()
        {
            setWindowState( Qt::WindowMinimized );
        }

        void Window::OnMaximizeClicked()
        {
            m_pRestoreButton->setVisible( true );
            m_pMaximizeButton->setVisible( false );
            setWindowState( Qt::WindowMaximized );
        }

        void Window::OnRestoreClicked()
        {
            m_pRestoreButton->setVisible( false );
            m_pMaximizeButton->setVisible( true );
            setWindowState( Qt::WindowNoState );
        }

        void Window::OnCloseClicked()
        {
            if ( OnCloseRequested() )
            {
                close();
            }
        }
    }
}