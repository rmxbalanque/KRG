#include "Editor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    #if KRG_DEVELOPMENT_TOOLS
    void Editor::InitializeDevelopmentUI()
    {
        m_editorUI.Initialize( m_settingsRegistry );
        m_pDevelopmentUI = &m_editorUI;
    }

    void Editor::ShutdownDevelopmentUI()
    {
        m_editorUI.Shutdown();
    }
    #endif
}