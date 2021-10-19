#include "RenderingSystem.h"
#include "Engine/Render/Renderers/MeshRenderer.h"
#include "Engine/Render/Renderers/ImguiRenderer.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Math/ViewVolume.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void RenderingSystem::Initialize( RenderDevice* pRenderDevice, ViewportManager* pViewportManager, RendererRegistry* pRegistry )
        {
            KRG_ASSERT( m_pRenderDevice == nullptr && m_pViewportManager == nullptr );
            KRG_ASSERT( pRenderDevice != nullptr && pViewportManager != nullptr && pRegistry != nullptr );

            m_pRenderDevice = pRenderDevice;
            m_pViewportManager = pViewportManager;

            //-------------------------------------------------------------------------

            for ( auto pRenderer : pRegistry->GetRegisteredRenderers() )
            {
                if ( pRenderer->GetRendererID() == MeshRenderer::RendererID )
                {
                    KRG_ASSERT( m_pMeshRenderer == nullptr );
                    m_pMeshRenderer = static_cast<MeshRenderer*>( pRenderer );
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
                #endif

                //-------------------------------------------------------------------------

                m_customRenderers.push_back( pRenderer );
            }

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
            m_pMeshRenderer = nullptr;

            #if KRG_DEVELOPMENT_TOOLS
            m_pImguiRenderer = nullptr;
            #endif

            m_pViewportManager = nullptr;
            m_pRenderDevice = nullptr;
        }

        void RenderingSystem::Update( UpdateContext const& ctx )
        {
            KRG_ASSERT( m_pRenderDevice != nullptr && m_pViewportManager != nullptr );

            auto const& immediateContext = m_pRenderDevice->GetImmediateContext();

            //-------------------------------------------------------------------------

            UpdateStage const updateStage = ctx.GetUpdateStage();
            KRG_ASSERT( updateStage != UpdateStage::FrameStart );

            switch ( updateStage )
            {
                case UpdateStage::PrePhysics:
                {
                    KRG_PROFILE_SCOPE_RENDER( "Rendering Pre-Physics" );

                    m_pViewportManager->UpdateCustomRenderTargets();

                    int32 const numViewports = m_pViewportManager->GetNumViewports();
                    for ( int32 i = 0; i < numViewports; i++ )
                    {
                        Render::Viewport const* pViewport = m_pViewportManager->GetActiveViewports()[i];
                        immediateContext.SetRenderTarget( m_pViewportManager->GetRenderTargetForViewport( i ) );

                        //-------------------------------------------------------------------------

                        if ( m_pMeshRenderer != nullptr )
                        {
                            m_pMeshRenderer->RenderStatic( *pViewport );
                        }
                    }
                }
                break;

                //-------------------------------------------------------------------------

                case UpdateStage::FrameEnd:
                {
                    KRG_PROFILE_SCOPE_RENDER( "Rendering Post-Physics" );

                    // Render into active viewports
                    //-------------------------------------------------------------------------

                    int32 const numViewports = m_pViewportManager->GetNumViewports();
                    for ( int32 i = 0; i < numViewports; i++ )
                    {
                        Render::Viewport const* pViewport = m_pViewportManager->GetActiveViewports()[i];
                        immediateContext.SetRenderTarget( m_pViewportManager->GetRenderTargetForViewport( i ) );

                        //-------------------------------------------------------------------------

                        if ( m_pMeshRenderer != nullptr )
                        {
                            m_pMeshRenderer->RenderDynamic( *pViewport );
                            m_pMeshRenderer->RenderSkeletal( *pViewport );
                        }

                        for ( auto const& pCustomRenderer : m_customRenderers )
                        {
                            pCustomRenderer->Render( *pViewport );
                        }
                    }

                    // Draw development tools viewport
                    //-------------------------------------------------------------------------

                    #if KRG_DEVELOPMENT_TOOLS
                    if ( m_pImguiRenderer != nullptr )
                    {
                        immediateContext.SetRenderTarget( m_pRenderDevice->GetPrimaryWindowRenderTarget() );

                        auto const& devToolsViewport = m_pViewportManager->GetDevelopmentToolsViewport();
                        m_pImguiRenderer->Render( devToolsViewport );
                    }
                    #endif

                    // Present frame
                    //-------------------------------------------------------------------------

                    m_pRenderDevice->PresentFrame();
                }
                break;
            }
        }
    }
}