#include "RenderingSystem.h"
#include "Engine/Render/Renderers/WorldRenderer.h"
#include "Engine/Render/Renderers/DebugRenderer.h"
#include "Engine/Render/Renderers/ImguiRenderer.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Math/ViewVolume.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void RenderingSystem::CreateCustomRenderTargetForViewport( Viewport const* pViewport )
    {
        KRG_ASSERT( pViewport != nullptr && pViewport->IsValid() );
        KRG_ASSERT( FindRenderTargetForViewport( pViewport ) == nullptr );

        auto& vrt = m_viewportRenderTargets.emplace_back();
        vrt.m_viewportID = pViewport->GetID();
        vrt.m_pRenderTarget = KRG::New<RenderTarget>();
        m_pRenderDevice->CreateRenderTarget( *vrt.m_pRenderTarget, pViewport->GetDimensions() );
        KRG_ASSERT( vrt.m_pRenderTarget->IsValid() );
    }

    void RenderingSystem::DestroyCustomRenderTargetForViewport( Viewport const* pViewport )
    {
        KRG_ASSERT( pViewport != nullptr && pViewport->GetID().IsValid() );

        auto pViewportRenderTarget = FindRenderTargetForViewport( pViewport );
        KRG_ASSERT( pViewportRenderTarget != nullptr );
        KRG_ASSERT( pViewportRenderTarget->m_pRenderTarget != nullptr && pViewportRenderTarget->m_pRenderTarget->IsValid() );

        m_pRenderDevice->LockDevice();
        {
            m_pRenderDevice->DestroyRenderTarget( *pViewportRenderTarget->m_pRenderTarget );
            KRG::Delete( pViewportRenderTarget->m_pRenderTarget );
        }
        m_pRenderDevice->UnlockDevice();

        m_viewportRenderTargets.erase( pViewportRenderTarget );
    }

    ViewSRVHandle const& RenderingSystem::GetRenderTargetTextureForViewport( Viewport const* pViewport ) const
    {
        KRG_ASSERT( pViewport != nullptr && pViewport->GetID().IsValid() );

        auto pViewportRenderTarget = FindRenderTargetForViewport( pViewport );
        KRG_ASSERT( pViewportRenderTarget != nullptr );
        KRG_ASSERT( pViewportRenderTarget->m_pRenderTarget != nullptr && pViewportRenderTarget->m_pRenderTarget->IsValid() );

        return pViewportRenderTarget->m_pRenderTarget->GetRenderTargetShaderResourceView();
    }

    RenderingSystem::ViewportRenderTarget* RenderingSystem::FindRenderTargetForViewport( Viewport const* pViewport )
    {
        auto SearchPredicate = [] ( ViewportRenderTarget const& viewportRenderTarget, UUID const& ID ) { return viewportRenderTarget.m_viewportID == ID; };
        auto viewportRenderTargetIter = eastl::find( m_viewportRenderTargets.begin(), m_viewportRenderTargets.end(), pViewport->GetID(), SearchPredicate );
        if ( viewportRenderTargetIter != m_viewportRenderTargets.end() )
        {
            return viewportRenderTargetIter;
        }

        return nullptr;
    }

    //-------------------------------------------------------------------------

    void RenderingSystem::Initialize( RenderDevice* pRenderDevice, Float2 primaryWindowDimensions, RendererRegistry* pRegistry, EntityWorldManager* pWorldManager )
    {
        KRG_ASSERT( m_pRenderDevice == nullptr );
        KRG_ASSERT( pRenderDevice != nullptr && pRegistry != nullptr );
        KRG_ASSERT( pWorldManager != nullptr );

        m_pRenderDevice = pRenderDevice;
        m_pWorldManager = pWorldManager;

        // Set initial render device size
        //-------------------------------------------------------------------------

        m_pRenderDevice->LockDevice();
        m_pRenderDevice->ResizePrimaryWindowRenderTarget( primaryWindowDimensions );
        m_pRenderDevice->UnlockDevice();

        // Initialize viewports
        //-------------------------------------------------------------------------

        Math::ViewVolume const orthographicVolume( primaryWindowDimensions, FloatRange( 0.1f, 100.0f ) );

        for ( auto pWorld : m_pWorldManager->GetWorlds() )
        {
            Render::Viewport* pViewport = pWorld->GetViewport();
            *pViewport = Viewport( Int2::Zero, primaryWindowDimensions, orthographicVolume );
        }

        #if KRG_DEVELOPMENT_TOOLS
        m_toolsViewport = Viewport( Int2::Zero, primaryWindowDimensions, orthographicVolume );
        #endif

        // Get renderers
        //-------------------------------------------------------------------------

        for ( auto pRenderer : pRegistry->GetRegisteredRenderers() )
        {
            if ( pRenderer->GetRendererID() == WorldRenderer::RendererID )
            {
                KRG_ASSERT( m_pWorldRenderer == nullptr );
                m_pWorldRenderer = static_cast<WorldRenderer*>( pRenderer );
                continue;
            }

            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            if ( pRenderer->GetRendererID() == ImguiRenderer::RendererID )
            {
                KRG_ASSERT( m_pImguiRenderer == nullptr );
                m_pImguiRenderer = static_cast<ImguiRenderer*>( pRenderer );
                continue;
            }

            if ( pRenderer->GetRendererID() == DebugRenderer::RendererID )
            {
                KRG_ASSERT( m_pDebugRenderer == nullptr );
                m_pDebugRenderer = static_cast<DebugRenderer*>( pRenderer );
                continue;
            }
            #endif

            //-------------------------------------------------------------------------

            m_customRenderers.push_back( pRenderer );
        }

        KRG_ASSERT( m_pWorldRenderer != nullptr );

        // Sort custom renderers
        //-------------------------------------------------------------------------

        auto comparator = [] ( IRenderer* const& pRendererA, IRenderer* const& pRendererB )
        {
            int32 const A = pRendererA->GetPriority();
            int32 const B = pRendererB->GetPriority();
            return A < B;
        };

        eastl::sort( m_customRenderers.begin(), m_customRenderers.end(), comparator );
    }

    void RenderingSystem::Shutdown()
    {
        // Destroy any viewport render targets created
        //-------------------------------------------------------------------------

        m_pRenderDevice->LockDevice();
        for ( auto& viewportRenderTarget : m_viewportRenderTargets )
        {
            m_pRenderDevice->DestroyRenderTarget( *viewportRenderTarget.m_pRenderTarget );
            KRG::Delete( viewportRenderTarget.m_pRenderTarget );
        }
        m_pRenderDevice->UnlockDevice();

        m_viewportRenderTargets.clear();

        // Clear ptrs
        //-------------------------------------------------------------------------

        m_pWorldRenderer = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        m_pImguiRenderer = nullptr;
        #endif

        m_pWorldManager = nullptr;
        m_pRenderDevice = nullptr;
    }

    void RenderingSystem::ResizePrimaryRenderTarget( Int2 newMainWindowDimensions )
    {
        Float2 const newWindowDimensions = Float2( newMainWindowDimensions );
        Float2 const oldWindowDimensions = Float2( m_pRenderDevice->GetPrimaryWindowDimensions() );

        //-------------------------------------------------------------------------

        m_pRenderDevice->LockDevice();
        m_pRenderDevice->ResizePrimaryWindowRenderTarget( newMainWindowDimensions );
        m_pRenderDevice->UnlockDevice();

        //-------------------------------------------------------------------------

        // TODO: add info whether viewports are in absolute size or proportional, right now it's all proportional
        for ( auto pWorld : m_pWorldManager->GetWorlds() )
        {
            Render::Viewport* pViewport = pWorld->GetViewport();
            Float2 const viewportPosition = pViewport->GetTopLeftPosition();
            Float2 const viewportSize = pViewport->GetDimensions();
            Float2 const newViewportPosition = ( viewportPosition / oldWindowDimensions ) * newWindowDimensions;
            Float2 const newViewportSize = ( viewportSize / oldWindowDimensions ) * newWindowDimensions;
            pViewport->Resize( newViewportPosition, newViewportSize );
        }
    }

    void RenderingSystem::Update( UpdateContext const& ctx )
    {
        KRG_ASSERT( m_pRenderDevice != nullptr );
        KRG_ASSERT( ctx.GetUpdateStage() == UpdateStage::FrameEnd );
        KRG_PROFILE_SCOPE_RENDER( "Rendering Post-Physics" );

        //-------------------------------------------------------------------------

        m_pRenderDevice->LockDevice();

        // Render into active viewports
        //-------------------------------------------------------------------------

        for ( auto pWorld : m_pWorldManager->GetWorlds() )
        {
            if ( pWorld->IsSuspended() )
            {
                continue;
            }

            Render::Viewport* pViewport = pWorld->GetViewport();
            ViewportRenderTarget* pVRT = FindRenderTargetForViewport( pViewport );

            // Update and set render target
            //-------------------------------------------------------------------------

            // If we are rendering to texture resize and clear the render target
            const RenderTarget* mainRT = &m_pRenderDevice->GetPrimaryWindowRenderTarget();
            if ( pVRT != nullptr )
            {
                auto pViewportRenderTarget = pVRT->m_pRenderTarget;

                // Resize render target if needed
                if ( Int2( pViewport->GetDimensions() ) != pViewportRenderTarget->GetDimensions() )
                {
                    m_pRenderDevice->ResizeRenderTarget( *pViewportRenderTarget, pViewport->GetDimensions() );
                }

                // Clear render target and depth stencil textures
                m_pRenderDevice->GetImmediateContext().ClearRenderTargetViews( *pViewportRenderTarget );
                mainRT = pViewportRenderTarget;
            }

            // Draw
            //-------------------------------------------------------------------------

            m_pWorldRenderer->RenderWorld( *mainRT, *pViewport, pWorld );

            for ( auto const& pCustomRenderer : m_customRenderers )
            {
                pCustomRenderer->RenderWorld( *mainRT, *pViewport, pWorld );
                pCustomRenderer->RenderViewport( *mainRT, *pViewport );
            }

            #if KRG_DEVELOPMENT_TOOLS
            m_pDebugRenderer->RenderWorld( *mainRT, *pViewport, pWorld );
            m_pDebugRenderer->RenderViewport( *mainRT, *pViewport );
            #endif
        }

        // Draw development UI
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_pImguiRenderer != nullptr )
        {
            m_pImguiRenderer->RenderViewport( m_pRenderDevice->GetPrimaryWindowRenderTarget(), m_toolsViewport );
        }
        #endif

        // Present frame
        //-------------------------------------------------------------------------

        m_pRenderDevice->PresentFrame();

        m_pRenderDevice->UnlockDevice();
    }
}