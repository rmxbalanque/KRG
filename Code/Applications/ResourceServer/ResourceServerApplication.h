#pragma once
#include "ResourceServer/ResourceServer.h"
#include "Tools/UI/Helpers/SingleInstanceLock.h"
#include "Tools/UI/Application/Window.h"
#include "Tools/UI/Application/Application.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class ResourceServerMainWindow : public Tools::Window
        {
        public:

            using Tools::Window::Window;

        private:

            virtual bool ShouldStartHidden() const override final { return true; }
            virtual void closeEvent( QCloseEvent* pEvent ) override final;
        };

        //-------------------------------------------------------------------------

        class ResourceServerApplication : public Tools::Application
        {
            Q_OBJECT

        public:

            ResourceServerApplication( int argc, char* argv[] );

        private:

            virtual bool Initialize() override final;
            virtual void Shutdown() override final;

            virtual Tools::Window* CreateMainWindow( QString const& windowName, QString const& windowIconPath, QString const& windowTitle ) override;

        private slots:

            void OnServerStopped();
            void OnTrayIconActivated( QSystemTrayIcon::ActivationReason reason );
            void OnServerBusyStateChanged();

            void CreateMainContent();
            void CreateStatusBarContent();

        private:

            // Tray Icon
            QSystemTrayIcon*                m_pTrayIcon = nullptr;
            QMenu*                          m_pTrayIconMenu = nullptr;
            QAction*                        m_pQuitAction = nullptr;
            QIcon                           m_idleIcon = QIcon( ":/KRG/TrayIconIdle.png" );
            QIcon                           m_busyIcon = QIcon( ":/KRG/TrayIconBusy.png" );

            // Server
            ResourceServer*                 m_pServer = nullptr;
            QThread*                        m_pServerThread = nullptr;
            Tools::SingleInstanceLock       m_instanceLock;
        };
    }
}