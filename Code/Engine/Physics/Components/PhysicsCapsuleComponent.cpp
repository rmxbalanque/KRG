#include "PhysicsCapsuleComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsCapsuleComponent::HasValidPhysicsSetup() const
    {
        if ( m_capsuleRadius <= 0 || m_capsuleHalfHeight <= 0 )
        {
            KRG_LOG_ERROR( "Invalid radius or half height on Physics Capsule Component: %s (%s). Negative or zero values are not allowed!", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        if ( m_pPhysicsMaterial == nullptr || !m_pPhysicsMaterial->IsValid() )
        {
            KRG_LOG_ERROR( "Invalid physical material setup on Physics Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        return true;
    }
}