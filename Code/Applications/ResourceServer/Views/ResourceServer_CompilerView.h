#pragma once
#include <QWidget>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class ResourceServer;

        //-------------------------------------------------------------------------

        class CompilerView : public QWidget
        {
            Q_OBJECT

        public:

            CompilerView( ResourceServer& resourceServer );

        private:

            ResourceServer const&       m_resourceServer;
        };
    }
}
