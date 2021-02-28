#pragma once
#include "../../_Module/API.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Types/ScopedValue.h"
#include "QWidget"
#include "QPushButton"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_UI_API ValueEditorBase : public QWidget
    {
        Q_OBJECT

    protected:

        // This is used to prevent value notification loops when updating the UI programatically
        bool            m_enableInputChangeHandlers = true;
    };
}