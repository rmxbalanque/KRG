#include "RenderingSystem.h"
#include "Engine/Render/Renderers/StaticMeshRenderer.h"
#include "Engine/Render/Renderers/SkeletalMeshRenderer.h"
#include "System/DevTools/Renderer/ImguiRenderer.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Render/RenderDevice/RenderDevice.h"
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
                if ( pRenderer->GetRendererID() == SkeletalMeshRenderer::RendererID )
                {
                    KRG_ASSERT( m_pSkeletalMeshRenderer == nullptr );
                    m_pSkeletalMeshRenderer = static_cast<SkeletalMeshRenderer*>( pRenderer );
                    continue;
                }
                
                if ( pRenderer->GetRendererID() == StaticMeshRenderer::RendererID )
                {
                    KRG_ASSERT( m_pStaticMeshRenderer == nullptr );
                    m_pStaticMeshRenderer = static_cast<StaticMeshRenderer*>( pRenderer );
                    continue;
                }

                //-------------------------------------------------------------------------

                #if KRG_DEVELOPMENT_TOOLS
                if ( pRenderer->GetRendererID() == ImGuiX::ImguiRenderer::RendererID )
                {
                    KRG_ASSERT( m_pImguiRenderer == nullptr );
                    m_pImguiRenderer = static_cast<ImGuiX::ImguiRenderer*>( pRenderer );
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
            m_pStaticMeshRenderer = nullptr;
            m_pSkeletalMeshRenderer = nullptr;

            #if KRG_DEVELOPMENT_TOOLS
            m_pImguiRenderer = nullptr;
            #endif

            m_pViewportManager = nullptr;
            m_pRenderDevice = nullptr;
        }

        void RenderingSystem::Update( UpdateContext const& ctx )
        {
            KRG_ASSERT( m_pRenderDevice != nullptr && m_pViewportManager != nullptr );

            //-------------------------------------------------------------------------

            UpdateStage const updateStage = ctx.GetUpdateStage();
            KRG_ASSERT( updateStage != UpdateStage::FrameStart );

            switch ( updateStage )
            {
                case UpdateStage::PrePhysics:
                {
                    KRG_PROFILE_SCOPE_RENDER( "Rendering Pre-Physics" );

                    for ( auto const& viewport : m_pViewportManager->GetActiveViewports() )
                    {
                        if ( m_pStaticMeshRenderer != nullptr )
                        {
                            m_pStaticMeshRenderer->RenderStatic( viewport );
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

                    for ( auto const& viewport : m_pViewportManager->GetActiveViewports() )
                    {
                        if ( m_pStaticMeshRenderer != nullptr )
                        {
                            m_pStaticMeshRenderer->RenderDynamic( viewport );
                        }

                        if ( m_pSkeletalMeshRenderer != nullptr )
                        {
                            m_pSkeletalMeshRenderer->Render( viewport );
                        }

                        for ( auto const& pCustomRenderer : m_customRenderers )
                        {
                            pCustomRenderer->Render( viewport );
                        }
                    }

                    // Draw development tools viewport
                    //-------------------------------------------------------------------------

                    #if KRG_DEVELOPMENT_TOOLS
                    if ( m_pImguiRenderer != nullptr )
                    {
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