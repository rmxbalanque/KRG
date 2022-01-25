#include "EntitySystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationClipPlayer.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Render/Components/Component_SkeletalMesh.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "System/Animation/AnimationPose.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationSystem::~AnimationSystem()
    {
        KRG_ASSERT( m_animGraphs.empty() && m_animPlayers.empty() && m_meshComponents.empty());
    }

    //-------------------------------------------------------------------------

    void AnimationSystem::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pMeshComponent = TryCast<Render::SkeletalMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( !VectorContains( m_meshComponents, pMeshComponent ) );
            m_meshComponents.push_back( pMeshComponent );
        }
        else if ( auto pAnimPlayerComponent = TryCast<AnimationClipPlayerComponent>( pComponent ) )
        {
            m_animPlayers.emplace_back( pAnimPlayerComponent );
        }
        else if ( auto pGraphComponent = TryCast<AnimationGraphComponent>( pComponent ) )
        {
            m_animGraphs.emplace_back( pGraphComponent );
        }
    }

    void AnimationSystem::UnregisterComponent( EntityComponent* pComponent )
    {
        if ( auto pMeshComponent = TryCast<Render::SkeletalMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( VectorContains( m_meshComponents, pMeshComponent ) );
            m_meshComponents.erase_first( pMeshComponent );
        }
        else if ( auto pAnimPlayerComponent = TryCast<AnimationClipPlayerComponent>( pComponent ) )
        {
            m_animPlayers.erase_first_unsorted( pAnimPlayerComponent );
        }
        else if ( auto pGraphComponent = TryCast<AnimationGraphComponent>( pComponent ) )
        {
            m_animGraphs.erase_first_unsorted( pGraphComponent );
        }
    }

    //-------------------------------------------------------------------------

    void AnimationSystem::Update( EntityWorldUpdateContext const& ctx )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();

        if ( m_animPlayers.empty() && m_animGraphs.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        Transform characterWorldTransform( NoInit );
        if ( m_meshComponents.empty() )
        {
            characterWorldTransform = Transform::Identity;
        }
        else
        {
            characterWorldTransform = m_meshComponents[0]->GetWorldTransform();
        }

        //-------------------------------------------------------------------------

        UpdateAnimPlayers( ctx, characterWorldTransform );
        UpdateAnimGraphs( ctx, characterWorldTransform );

        //-------------------------------------------------------------------------

        for ( auto pMeshComponent : m_meshComponents )
        {
            pMeshComponent->FinalizePose();
        }
    }

    void AnimationSystem::UpdateAnimPlayers( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform )
    {
        UpdateStage const updateStage = ctx.GetUpdateStage();
        if ( updateStage == UpdateStage::PrePhysics )
        {
            for ( auto pAnimComponent : m_animPlayers )
            {
                if ( !pAnimComponent->RequiresManualUpdate() )
                {
                    pAnimComponent->Update( ctx.GetDeltaTime(), characterWorldTransform );
                }

                //-------------------------------------------------------------------------

                auto const* pPose = pAnimComponent->GetPose();
                KRG_ASSERT( pPose->HasGlobalTransforms() );

                for ( auto pMeshComponent : m_meshComponents )
                {
                    if ( !pMeshComponent->HasMeshResourceSet() )
                    {
                        continue;
                    }

                    if ( pPose->GetSkeleton() != pMeshComponent->GetSkeleton() )
                    {
                        continue;
                    }

                    pMeshComponent->SetPose( pPose );
                }
            }
        }
    }

    void AnimationSystem::UpdateAnimGraphs( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform )
    {
        UpdateStage const updateStage = ctx.GetUpdateStage();
        if ( updateStage == UpdateStage::PrePhysics )
        {
            auto pPhysicsWorldSystem = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();

            for ( auto pAnimComponent : m_animGraphs )
            {
                if ( !pAnimComponent->RequiresManualUpdate() )
                {
                    pAnimComponent->EvaluateGraph( ctx.GetDeltaTime(), characterWorldTransform, pPhysicsWorldSystem->GetScene() );
                    // TODO: apply root motion here!
                    pAnimComponent->ExecutePrePhysicsTasks( characterWorldTransform );
                }
            }
        }
        else if ( updateStage == UpdateStage::PostPhysics )
        {
            for ( auto pAnimComponent : m_animGraphs )
            {
                if ( !pAnimComponent->RequiresManualUpdate() )
                {
                    pAnimComponent->ExecutePostPhysicsTasks();
                }

                //-------------------------------------------------------------------------

                auto const* pPose = pAnimComponent->GetPose();
                KRG_ASSERT( pPose->HasGlobalTransforms() );

                for ( auto pMeshComponent : m_meshComponents )
                {
                    if ( !pMeshComponent->HasMeshResourceSet() )
                    {
                        continue;
                    }

                    if ( pPose->GetSkeleton() != pMeshComponent->GetSkeleton() )
                    {
                        continue;
                    }

                    pMeshComponent->SetPose( pPose );
                }
            }
        }
    }
}