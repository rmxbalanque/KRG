#include "AnimationSystem.h"
#include "Engine/Animation/Components/AnimationComponent.h"
#include "Engine/Animation/Components/AnimatedMeshComponent.h"
#include "System/Animation/AnimationPose.h"
#include "System/Core/Profiling/Profiling.h"
#include "Engine/Animation/Debug/AnimDebugDrawing.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        UpdatePriorityList const AnimationSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::FrameStart );

        //-------------------------------------------------------------------------

        AnimationSystem::~AnimationSystem()
        {
            KRG_ASSERT( m_pAnimComponent == nullptr && m_meshComponents.empty() );
        }

        //-------------------------------------------------------------------------

        void AnimationSystem::RegisterComponent( EntityComponentPtr pComponent )
        {
            if ( auto pMeshComponent = TEntityComponentPtr<AnimatedMeshComponent>( pComponent ) )
            {
                KRG_ASSERT( !VectorContains( m_meshComponents, pMeshComponent.GetRawPtr() ) );
                m_meshComponents.push_back( pMeshComponent.GetRawPtr() );
            }
            else if ( auto pAnimComponent = TEntityComponentPtr<AnimationComponent>( pComponent ) )
            {
                m_pAnimComponent = pAnimComponent.GetRawPtr();
            }
        }

        void AnimationSystem::UnregisterComponent( EntityComponentPtr pComponent )
        {
            if ( auto pMeshComponent = TEntityComponentPtr<AnimatedMeshComponent>( pComponent ) )
            {
                KRG_ASSERT( VectorContains( m_meshComponents, pMeshComponent.GetRawPtr() ) );
                m_meshComponents.erase_first( pMeshComponent.GetRawPtr() );
            }
            else if ( auto pAnimComponent = TEntityComponentPtr<AnimationComponent>( pComponent ) )
            {
                m_pAnimComponent = nullptr;
            }
        }

        //-------------------------------------------------------------------------

        void AnimationSystem::Update( UpdateContext const& ctx )
        {
            KRG_PROFILE_GROUPED_SCOPE_COLOR( "Animation", "AnimationSystem::Update", MP_THISTLE );

            if ( m_pAnimComponent == nullptr || m_meshComponents.empty() )
            {
                return;
            }

            //-------------------------------------------------------------------------

            m_pAnimComponent->Update( ctx );

            auto const* pPose = m_pAnimComponent->GetPose();
            KRG_ASSERT( pPose->HasGlobalTransforms() );

            /*auto drawingCtx = ctx.GetDrawingContext();
            DrawPose( drawingCtx, pPose, m_meshComponents[0]->GetWorldTransform() );*/

            //-------------------------------------------------------------------------

            for ( auto pMeshComponent : m_meshComponents )
            {
                if ( pPose->GetSkeleton() != pMeshComponent->GetSkeleton() )
                {
                    continue;
                }

                pMeshComponent->SetPose( pPose );
                pMeshComponent->FinalizePose();
            }
        }
    }
}