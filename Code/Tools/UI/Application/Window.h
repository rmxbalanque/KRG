#pragma once

#include "Tools/UI/_Module/API.h"
#include <QApplication>
#include <QtWidgets>
#include <QMenuBar>
#include <QSplashScreen>
#include <QMainWindow>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        class KRG_TOOLS_UI_API Window : public QMainWindow
        {
            Q_OBJECT

        public:

            Window( QString const& windowName, QString const& windowIconPath, QString const& windowTitle );

            // Shows the window and restores the save previous state, call this function and not the default show()
            void ShowWindow();

            // Symmetric hide window call
            void HideWindow() { QMainWindow::hide(); }

            // Should this window start hidden
            virtual bool ShouldStartHidden() const { return false; }

            // Content Regions
            inline QWidget* GetTitleBarContentWidgetLeft() { return m_pTitleBarContentLeft; }
            inline QWidget* GetTitleBarContentWidgetRight() { return m_pTitleBarContentRight; }
            inline QWidget* GetMainContentWidget() { return m_pMainContent; }
            inline QStatusBar* GetStatusBar() { return m_pStatusBar; }

        protected:

            // Called when we receive a close event, return true to close the window, false to cancel
            virtual bool OnCloseRequested() { return true; }

            // QT functions
            virtual bool nativeEvent( const QByteArray& eventType, void* pMessageData, long* pResult ) override;
            virtual void closeEvent( QCloseEvent* pEvent ) override;
            virtual void changeEvent( QEvent* pEvent ) override;

        private slots:

            void OnMinimizeClicked();
            void OnMaximizeClicked();
            void OnRestoreClicked();
            void OnCloseClicked();

        private:

            QWidget* CreateTitleBar( QString const& windowIconPath, QString const& windowTitle );

        protected:

            QString             m_windowName;

        private:

            QWidget*            m_pTitleBar = nullptr;
            QLabel*             m_pTitleBarIcon = nullptr;
            QLabel*             m_pTitleBarLabel = nullptr;

            QPushButton*        m_pMinimizeButton = nullptr;
            QPushButton*        m_pMaximizeButton = nullptr;
            QPushButton*        m_pRestoreButton = nullptr;
            QPushButton*        m_pCloseButton = nullptr;

            // Content Regions
            QWidget*            m_pTitleBarContentLeft = nullptr;
            QWidget*            m_pTitleBarContentRight = nullptr;
            QWidget*            m_pMainContent = nullptr;
            QStatusBar*         m_pStatusBar = nullptr;
        };
    }
}