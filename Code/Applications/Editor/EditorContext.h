#pragma once

//-------------------------------------------------------------------------

namespace ads { class CDockManager; }
class QMenuBar;

//-------------------------------------------------------------------------

namespace KRG
{
    struct EditorContext
    {
        ads::CDockManager*          m_pDockManager = nullptr;
        QMenuBar*                   m_pMainMenu = nullptr;
    };
}