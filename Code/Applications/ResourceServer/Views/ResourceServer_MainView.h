#pragma once

#include "QWidget"
#include "System/Core/Core.h"

//-------------------------------------------------------------------------

class QStackedLayout;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class ResourceServer;

        //-------------------------------------------------------------------------

        class MainViewWidget : public QWidget
        {
            Q_OBJECT

        signals:

            void ChangePageIndex( S32 pageIdx );

        public:

            MainViewWidget( ResourceServer& resourceServer );

        private slots:

            void OnChangePageIndex( S32 pageIdx );

            QWidget* CreateCompilerPage( ResourceServer& resourceServer );
            QWidget* CreateRequestsPage( ResourceServer& resourceServer );

        private:

            QStackedLayout*                     m_pStackedLayout = nullptr;
        };
    }
}