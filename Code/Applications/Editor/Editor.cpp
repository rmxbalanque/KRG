#include "Editor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    #if KRG_DEVELOPMENT_TOOLS
    void Editor::InitializeDevelopmentTools()
    {
        m_editorTools.Initialize( m_settingsRegistry );
        m_pDevelopmentTools = &m_editorTools;
    }

    void Editor::ShutdownDevelopmentTools()
    {
        m_editorTools.Shutdown();
    }
    #endif
}