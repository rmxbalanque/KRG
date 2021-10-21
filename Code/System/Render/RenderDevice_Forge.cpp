#include "RenderDevice_Forge.h"

//-------------------------------------------------------------------------

#if _WIN32
#pragma warning( push )
#pragma warning( disable: 4800 4324 4005 )
#pragma warning( disable: 4800 )
#endif

#include "Renderer/IResourceLoader.h"
#include "Renderer/IRenderer.h"

#if _WIN32
#pragma warning( pop )
#endif

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void InitializeRenderSystem()
    {
        if ( !initMemAlloc( "ForgeTest" ) )
        {
            return;
        }

        fsSetPathForResourceDir( pSystemFileIO, RM_DEBUG, RD_LOG, "" );

        initLog( "ForgeTest", DEFAULT_LOG_LEVEL );
    }

    void ShutdownRenderSystem()
    {
        exitLog();
        exitMemAlloc();
    }

    //-------------------------------------------------------------------------

    RenderDeviceForge::~RenderDeviceForge()
    {
        KRG_ASSERT( m_pRenderer == nullptr );
    }

    bool RenderDeviceForge::Initialize( Settings const& settings )
    {
        RendererDesc renderDesc;
        memset( &renderDesc, 0, sizeof( renderDesc ) );
        initRenderer( "KrugerRenderer", &renderDesc, &m_pRenderer );
        return true;
    }

    void RenderDeviceForge::Shutdown()
    {
        exitRenderer( m_pRenderer );
        m_pRenderer = nullptr;
    }
}