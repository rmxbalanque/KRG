#include "EntitySystem_AIController.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "System/Core/Math/Random.h"

#include <bfxMoverSpace.h>
#include "../Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
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
            m_pMover = pNavmeshSystem->CreateMover( ctx.GetSystem<TaskSystem>() );
        }

        if ( m_pMover != nullptr && m_pMover->IsIdle() )
        {
            Vector randomPos( Math::GetRandomFloat( -40, 40 ), Math::GetRandomFloat( -40, 40 ), 0.2f );
            pNavmeshSystem->SetMoverGoal( m_pMover, randomPos );
            //m_pMover->GotoPos( Navmesh::ToBfx( randomPos ) );
        }
    }
}