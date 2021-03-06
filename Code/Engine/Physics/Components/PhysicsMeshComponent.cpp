#include "PhysicsMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsMeshComponent::HasValidPhysicsSetup() const
    {
        if ( !m_pPhysicsMesh->IsValid() )
        {
            KRG_LOG_ERROR( "Invalid collision mesh on Physics Mesh Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        if ( m_pPhysicsMaterial == nullptr || !m_pPhysicsMaterial->IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid primary physical material setup on Physics Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        if ( m_pPhysicsMesh->IsTriangleMesh() )
        {
            uint16 const numMaterialsNeeded = m_pPhysicsMesh->GetNumMaterialsNeeded();

            // Ensure we have the correct number of materials set
            if ( ( m_additionalPhysicsMaterials.size() + 1 ) != numMaterialsNeeded )
            {
                //KRG_LOG_WARNING( "Physics", "Required number of additional physical materials no set on Physics Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            }

            // Check additional materials
            for ( auto i = 0; i < m_additionalPhysicsMaterials.size(); i++ )
            {
                if ( m_additionalPhysicsMaterials[i] == nullptr || !m_additionalPhysicsMaterials[i]->IsValid() )
                {
                    //KRG_LOG_WARNING( "Physics", "Invalid additional physical material (#%d) setup on Physics Component: %s (%s)", i, GetName().c_str(), GetID().ToString().c_str() );
                }
            }
        }

        return true;
    }

    TInlineVector<physx::PxMaterial*, 4> PhysicsMeshComponent::GetPhysicalMaterials() const
    {
        TInlineVector<physx::PxMaterial*, 4> physicalMaterials;
        physicalMaterials.emplace_back( m_pPhysicsMaterial->GetMaterial() );

        if ( m_pPhysicsMesh->IsTriangleMesh() )
        {
            uint16 const numMaterialsNeeded = m_pPhysicsMesh->GetNumMaterialsNeeded();
            for ( int32 i = 0; i < numMaterialsNeeded - 1; i++ )
            {
                if ( i < m_additionalPhysicsMaterials.size() )
                {
                    if ( m_additionalPhysicsMaterials[i] != nullptr && !m_additionalPhysicsMaterials[i]->IsValid() )
                    {
                        physicalMaterials.emplace_back( m_additionalPhysicsMaterials[i]->GetMaterial() );
                    }
                    else // Use primary in place of missing material
                    {
                        physicalMaterials.emplace_back( m_pPhysicsMaterial->GetMaterial() );
                    }
                }
                else // Use primary in place of missing material
                {
                    physicalMaterials.emplace_back( m_pPhysicsMaterial->GetMaterial() );
                }
            }
        }

        return physicalMaterials;
    }
}