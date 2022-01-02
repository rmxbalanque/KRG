#include "PhysicsScene.h"
#include "PhysicsRagdoll.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    Scene::Scene( physx::PxScene* pScene )
        : m_pScene( pScene )
    {
        KRG_ASSERT( pScene != nullptr );
    }

    Scene::~Scene()
    {
        m_pScene->release();
        m_pScene = nullptr;
    }

    Ragdoll* Scene::CreateRagdoll( RagdollDefinition const* pDefinition, StringID const profileID, uint64 userID )
    {
        KRG_ASSERT( m_pScene != nullptr && pDefinition != nullptr );
        auto pRagdoll = KRG::New<Ragdoll>( &m_pScene->getPhysics(), pDefinition, profileID, userID );
        pRagdoll->AddToScene( m_pScene );
        return pRagdoll;
    }
}