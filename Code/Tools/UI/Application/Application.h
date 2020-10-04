#pragma once

#include "Window.h"
#include "System/Core/Core.h"

#include <QApplication>
#include <QSplashScreen>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        class KRG_TOOLS_UI_API Application : public QObject
        {
            // Needs to be a QObject to allow connecting to UI signals
            Q_OBJECT

        public:

            Application( int argc, char* argv[] );
            virtual ~Application();

            inline int Run();

        protected:

            void ShowSplashScreen();
            void HideSplashScreen();

            // This is called just after the splash screen is shown, allows you to start any systems you may need
            virtual bool Initialize() = 0;

            // This is called as the application is about to quit
            virtual void Shutdown() = 0;

            // This is called after initialize and requires the client to create the main window for the application
            virtual Window* CreateMainWindow( QString const& windowName, QString const& windowIconPath, QString const& windowTitle ) { return new Window( windowName, windowIconPath, windowTitle ); }

            //-------------------------------------------------------------------------

            bool FatalError( QString const& errorMessage ) const;

        protected:

            QString                     m_applicationName;
            QString                     m_applicationIconPath;
            QString                     m_applicationVersion;
            QString                     m_splashScreenPath;

            Window*                     m_pWindow = nullptr;

        private:

            QApplication*               m_pApplication = nullptr;
            QSplashScreen*              m_pSplashScreen = nullptr;
        };
    }
}