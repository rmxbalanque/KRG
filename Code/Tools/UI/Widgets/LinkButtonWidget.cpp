#include "LinkButtonWidget.h"
#include <QApplication>

//-------------------------------------------------------------------------

namespace KRG
{
    void KLinkButton::leaveEvent( QEvent* pEvent )
    {
        QApplication::restoreOverrideCursor();
        QPushButton::leaveEvent( pEvent );
    }

    void KLinkButton::enterEvent( QEvent* pEvent )
    {
        QPushButton::enterEvent( pEvent );
        QApplication::setOverrideCursor( Qt::PointingHandCursor );
    }
}