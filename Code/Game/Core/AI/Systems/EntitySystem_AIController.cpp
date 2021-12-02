#include "EntitySystem_AIController.h"
//#include "Engine/Navmesh/NavPower.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    AIController::~AIController()
    {
    }

    //-------------------------------------------------------------------------

    void AIController::Activate()
    {
        //m_pMover = Navmesh::CreateMover( Vector::Zero, Quaternion::Identity, &m_tune );
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
        
    }
}