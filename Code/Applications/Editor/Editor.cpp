#include "Editor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void Editor::OnWindowResize( Int2 const& windowDimensions )
    {
        KRG_ASSERT( m_initialized );
        m_pRenderDevice->ResizeRenderTargets( windowDimensions );
        m_pRenderViewportSystem->ResizeDevelopmentToolsViewport( windowDimensions );
    }

    #if KRG_DEVELOPMENT_TOOLS
    void Editor::InitializeDevelopmentUI()
    {
        m_editorUI.Initialize( m_settingsRegistry );
        m_pDevelopmentUI = &m_editorUI;
        m_pRenderViewportSystem->CreateDevelopmentToolsViewport( m_pRenderDevice->GetRenderTargetDimensions() );
    }

    void Editor::ShutdownDevelopmentUI()
    {
        m_editorUI.Shutdown();
    }
    #endif
}