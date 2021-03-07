#include "PhysicsSphereComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsSphereComponent::HasValidPhysicsSetup() const
    {
        if ( m_radius <= 0.0f )
        {
            KRG_LOG_ERROR( "Physics", "Invalid radius on Physics Sphere Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        if ( !m_physicsMaterialID.IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid physical material setup on Physics Component: %s (%s)", GetName().c_str(), GetID().ToString().c_str() );
            return false;
        }

        return true;
    }
}