#include "PhysicsMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsMeshComponent::HasValidPhysicsSetup() const
    {
        if ( !m_pPhysicsMesh.IsValid() )
        {
            return false;
        }

        KRG_ASSERT( m_pPhysicsMesh.IsLoaded() );

        if ( !m_pPhysicsMesh->IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid collision mesh on Physics Mesh Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        return true;
    }

    TInlineVector<StringID, 4> PhysicsMeshComponent::GetPhysicsMaterialIDs() const
    {
        TInlineVector<StringID, 4> materials = m_pPhysicsMesh->GetPhysicsMaterials();

        //-------------------------------------------------------------------------

        uint16 const numMaterialsNeeded = m_pPhysicsMesh->GetNumMaterialsNeeded();
        for ( int32 i = 0; i < numMaterialsNeeded; i++ )
        {
            if ( i < m_physicsMaterialOverrideIDs.size() && m_physicsMaterialOverrideIDs[i].IsValid() )
            {
                materials[i] = m_physicsMaterialOverrideIDs[i];
            }
        }

        return materials;
    }

    void PhysicsMeshComponent::Initialize()
    {
        PhysicsComponent::Initialize();

        KRG_ASSERT( !m_pPhysicsMesh.IsValid() || m_pPhysicsMesh.IsLoaded() );
    }
}