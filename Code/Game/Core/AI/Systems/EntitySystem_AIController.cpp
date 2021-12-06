#include "EntitySystem_AIController.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Physics/PhysicsStateController.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/Random.h"

// Physics States
#include "Game/Core/AI/PhysicsStates/AIPhysicsState_AnimDriven.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AIPhysicsController final : public Physics::PhysicsStateController
    {
        virtual void CreatePhysicsStatesInternal() override
        {
            m_registeredStates.emplace_back( KRG::New<AnimDrivenPhysicsState>() );
            m_pActiveState = m_registeredStates.back();
            m_pActiveState->Activate();
        }
    };

    //-------------------------------------------------------------------------

    AIController::~AIController()
    {
    }

    //-------------------------------------------------------------------------

    void AIController::Activate()
    {
        
    }

    void AIController::Deactivate()
    {
        //Navmesh::DestroyMover( m_pMover );
        //m_pMover = nullptr;
    }

    //-------------------------------------------------------------------------

    void AIController::RegisterComponent( EntityComponent* pComponent )
    {
        
    }

    void AIController::UnregisterComponent( EntityComponent* pComponent )
    {
        
    }

    //-------------------------------------------------------------------------

    void AIController::Update( EntityUpdateContext const& ctx )
    {
        auto pNavmeshSystem = ctx.GetWorldSystem<Navmesh::NavmeshWorldSystem>();

        if ( m_pMover == nullptr )
        {
            m_pMover = pNavmeshSystem->CreateMover( );
        }

        if ( m_pMover != nullptr && m_pMover->IsIdle() )
        {
            Vector randomPos( Math::GetRandomFloat( -40, 40 ), Math::GetRandomFloat( -40, 40 ), 0.2f );
            pNavmeshSystem->SetMoverGoal( m_pMover, randomPos );
            //m_pMover->GotoPos( Navmesh::ToBfx( randomPos ) );
        }
    }
}