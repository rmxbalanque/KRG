#pragma once

#include "Tools/UI/_Module/API.h"
#include <QPushButton>

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_UI_API KLinkButton : public QPushButton
    {
        Q_OBJECT

        using QPushButton::QPushButton;

    protected:

        virtual void enterEvent( QEvent* pEvent ) override final;
        virtual void leaveEvent( QEvent* pEvent ) override final;
    };
}