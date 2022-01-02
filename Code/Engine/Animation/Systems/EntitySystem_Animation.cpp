#include "EntitySystem_Animation.h"
#include "Engine/Animation/Components/Component_Animation.h"
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
        KRG_ASSERT( m_pAnimComponent == nullptr && m_meshComponents.empty() );
    }

    //-------------------------------------------------------------------------

    void AnimationSystem::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pMeshComponent = TryCast<Render::SkeletalMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( !VectorContains( m_meshComponents, pMeshComponent ) );
            m_meshComponents.push_back( pMeshComponent );
        }
        else if ( auto pAnimComponent = TryCast<AnimationComponent>( pComponent ) )
        {
            if ( m_pAnimComponent == nullptr )
            {
                m_pAnimComponent = pAnimComponent;
            }
            else // For now we dont support multiple animation components on one character
            {
                KRG_LOG_WARNING( "Animation", "Multiple animation components detected for entity: %u", pAnimComponent->GetEntityID() );
            }
        }
    }

    void AnimationSystem::UnregisterComponent( EntityComponent* pComponent )
    {
        if ( auto pMeshComponent = TryCast<Render::SkeletalMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( VectorContains( m_meshComponents, pMeshComponent ) );
            m_meshComponents.erase_first( pMeshComponent );
        }
        else if ( auto pAnimComponent = TryCast<AnimationComponent>( pComponent ) )
        {
            if ( pAnimComponent == m_pAnimComponent )
            {
                m_pAnimComponent = nullptr;
            }
        }
    }

    //-------------------------------------------------------------------------

    void AnimationSystem::Update( EntityWorldUpdateContext const& ctx )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();

        if ( m_pAnimComponent == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto GetMeshWorldTransform = [this] ()
        {
            if ( m_meshComponents.empty() )
            {
                return Transform::Identity;
            }
            else
            {
                return m_meshComponents[0]->GetWorldTransform();
            }
        };

        //-------------------------------------------------------------------------

        auto pPhysicsWorldSystem = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();

        UpdateStage const updateStage = ctx.GetUpdateStage();
        if ( updateStage == UpdateStage::PrePhysics )
        {
            if ( !m_pAnimComponent->RequiresManualUpdate() )
            {
                m_pAnimComponent->PrePhysicsUpdate( ctx.GetDeltaTime(), GetMeshWorldTransform(), pPhysicsWorldSystem->GetScene() );
            }
        }
        else if ( updateStage == UpdateStage::PostPhysics )
        {
            if ( !m_pAnimComponent->RequiresManualUpdate() )
            {
                m_pAnimComponent->PostPhysicsUpdate( ctx.GetDeltaTime(), GetMeshWorldTransform(), pPhysicsWorldSystem->GetScene() );
            }

            // Transfer Pose
            //-------------------------------------------------------------------------

            auto const* pPose = m_pAnimComponent->GetPose();
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
                pMeshComponent->FinalizePose();
            }
        }
    }
}