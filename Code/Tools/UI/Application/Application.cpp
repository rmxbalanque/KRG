#include "Application.h"
#include "Tools/UI/Style/DarkStyle.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        Application::Application( int argc, char* argv[] )
        {
            QApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
            QApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );

            //-------------------------------------------------------------------------

            m_pApplication = new QApplication( argc, argv );
            KRG_ASSERT( m_pApplication != nullptr );
            m_pApplication->setOrganizationName( "" );
            m_pApplication->setOrganizationDomain( "" );

            Style::InitializeDarkStyle();
        }

        Application::~Application()
        {
            KRG_ASSERT( m_pSplashScreen == nullptr && m_pWindow == nullptr );

            Style::ShutdownDarkStyle();

            delete m_pApplication;
            m_pApplication = nullptr;
        }

        int Application::Run()
        {
            int result = -1;

            KRG_ASSERT( !m_applicationName.isEmpty() && !m_applicationVersion.isEmpty() );

            m_pApplication->setApplicationName( m_applicationName );
            m_pApplication->setApplicationVersion( m_applicationVersion );

            // Show the splash screen and initialize application systems
            //-------------------------------------------------------------------------

            ShowSplashScreen();

            m_pWindow = CreateMainWindow( m_applicationName, m_applicationIconPath, m_applicationName );

            if( !Initialize() )
            {
                return result;
            }

            // Create main window
            //-------------------------------------------------------------------------

            m_pApplication->processEvents();

            // Hide splash screen and show main window
            //-------------------------------------------------------------------------

            HideSplashScreen();

            if ( !m_pWindow->ShouldStartHidden() )
            {
                m_pWindow->ShowWindow();
            }

            //-------------------------------------------------------------------------

            result = m_pApplication->exec();

            // Shutdown window and application systems
            //-------------------------------------------------------------------------

            delete m_pWindow;
            m_pWindow = nullptr;

            Shutdown();

            //-------------------------------------------------------------------------

            return result;
        }

        //-------------------------------------------------------------------------

        void Application::ShowSplashScreen()
        {
            if ( !m_splashScreenPath.isEmpty() )
            {
                m_pSplashScreen = new QSplashScreen();
                QPixmap pixmap( m_splashScreenPath );
                m_pSplashScreen->setPixmap( pixmap );
                m_pSplashScreen->showMessage( "Version: " + m_applicationVersion, Qt::AlignBottom | Qt::AlignRight, QColor( 255, 255, 255 ) );
                m_pSplashScreen->show();
            }
        }

        void Application::HideSplashScreen()
        {
            if ( m_pSplashScreen != nullptr )
            {
                m_pSplashScreen->finish( m_pWindow );
                delete m_pSplashScreen;
                m_pSplashScreen = nullptr;
            }
        }

        //-------------------------------------------------------------------------

        bool Application::FatalError( QString const& errorMessage ) const
        {
            QMessageBox messageBox;
            messageBox.setFixedSize( 500, 200 );
            messageBox.critical( 0, "Fatal Error", errorMessage );
            return false;
        }
    }
}