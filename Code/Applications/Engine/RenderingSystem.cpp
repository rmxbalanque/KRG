#include "RenderingSystem.h"
#include "Engine/Render/Renderers/StaticMeshRenderer.h"
#include "Engine/Render/Renderers/SkeletalMeshRenderer.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void RenderingSystem::Initialize()
        {
            for ( auto pRenderer : m_rendererRegistry.GetRegisteredRenderers() )
            {
                if ( pRenderer->GetRendererID() == SkeletalMeshRenderer::RendererID )
                {
                    KRG_ASSERT( m_pSkeletalMeshRenderer == nullptr );
                    m_pSkeletalMeshRenderer = static_cast<SkeletalMeshRenderer*>( pRenderer );
                }
                else if ( pRenderer->GetRendererID() == StaticMeshRenderer::RendererID )
                {
                    KRG_ASSERT( m_pStaticMeshRenderer == nullptr );
                    m_pStaticMeshRenderer = static_cast<StaticMeshRenderer*>( pRenderer );
                }
                else
                {
                    m_customRenderers.push_back( pRenderer );
                }
            }

            // Sort custom renderers
            //-------------------------------------------------------------------------

            auto comparator = [] ( IRenderer* const& pRendererA, IRenderer* const& pRendererB )
            {
                S32 const A = pRendererA->GetPriority();
                S32 const B = pRendererB->GetPriority();
                return A < B;
            };

            eastl::sort( m_customRenderers.begin(), m_customRenderers.end(), comparator );
        }

        void RenderingSystem::Shutdown()
        {
            m_pStaticMeshRenderer = nullptr;
            m_pSkeletalMeshRenderer = nullptr;
        }

        void RenderingSystem::Update( UpdateContext const& ctx, TInlineVector<Math::Viewport, 2> activeViewports )
        {
            UpdateStage const updateStage = ctx.GetUpdateStage();
            KRG_ASSERT( updateStage != UpdateStage::FrameStart );

            //-------------------------------------------------------------------------

            switch ( updateStage )
            {
                case UpdateStage::PrePhysics:
                {
                    KRG_PROFILE_GROUPED_SCOPE_COLOR( "Render", "RenderingSystem::Update", MP_PINK );

                    for ( auto const& viewport : activeViewports )
                    {
                        if ( m_pStaticMeshRenderer != nullptr )
                        {
                            m_pStaticMeshRenderer->RenderStatic( viewport );
                        }
                    }
                }
                break;

                case UpdateStage::FrameEnd:
                {
                    KRG_PROFILE_GROUPED_SCOPE_COLOR( "Render", "RenderingSystem::Update", MP_PINK );

                    for ( auto const& viewport : activeViewports )
                    {
                        if ( m_pStaticMeshRenderer != nullptr )
                        {
                            m_pStaticMeshRenderer->RenderDynamic( viewport );
                        }

                        if ( m_pSkeletalMeshRenderer != nullptr )
                        {
                            m_pSkeletalMeshRenderer->Render( viewport );
                        }

                        for ( auto const& pDebugRenderer : m_customRenderers )
                        {
                            pDebugRenderer->Render( viewport );
                        }
                    }
                }
                break;
            }
        }
    }
}