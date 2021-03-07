#include "PhysicsMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsMeshComponent::HasValidPhysicsSetup() const
    {
        if ( !m_pPhysicsMesh->IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid collision mesh on Physics Mesh Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        if ( !m_physicsMaterialID.IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid primary physical material setup on Physics Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        return true;
    }

    TInlineVector<StringID, 4> PhysicsMeshComponent::GetPhysicsMaterialIDs() const
    {
        TInlineVector<StringID, 4> materials;
        materials.emplace_back( m_physicsMaterialID );

        if ( m_pPhysicsMesh->IsTriangleMesh() )
        {
            uint16 const numMaterialsNeeded = m_pPhysicsMesh->GetNumMaterialsNeeded();
            for ( int32 i = 0; i < numMaterialsNeeded - 1; i++ )
            {
                bool validMaterialFound = false;
                if ( i < m_additionalPhysicsMaterialIDs.size() )
                {
                    if ( m_additionalPhysicsMaterialIDs[i].IsValid() )
                    {
                        materials.emplace_back( m_additionalPhysicsMaterialIDs[i] );
                        validMaterialFound = true;
                    }
                }

                // Use primary in place of missing material
                if( !validMaterialFound )
                {
                    materials.emplace_back( m_physicsMaterialID );
                }
            }
        }

        return materials;
    }
}