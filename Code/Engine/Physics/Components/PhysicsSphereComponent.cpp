#include "PhysicsSphereComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    // This constructor only exists to lazy initialize the static default material ID
    PhysicsSphereComponent::PhysicsSphereComponent()
    {
        static StringID const defaultMaterialID( PhysicsMaterial::DefaultID );
        m_physicsMaterialID = defaultMaterialID;
    }

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