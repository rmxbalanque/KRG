#include "RenderingSystem.h"
#include "Engine/Render/Renderers/StaticMeshRenderer.h"
#include "Engine/Render/Renderers/SkeletalMeshRenderer.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void RenderingSystem::Initialize( Render::RendererRegistry* pRegistry )
        {
            KRG_ASSERT( pRegistry != nullptr );

            for ( auto pRenderer : pRegistry->GetRegisteredRenderers() )
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
                    KRG_PROFILE_SCOPE_RENDER( "Rendering Pre-Physics" );

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
                    KRG_PROFILE_SCOPE_RENDER( "Rendering Post-Physics" );

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