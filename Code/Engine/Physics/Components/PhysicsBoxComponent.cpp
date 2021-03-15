#include "PhysicsBoxComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    // This constructor only exists to lazy initialize the static default material ID
    PhysicsBoxComponent::PhysicsBoxComponent()
    {
        static StringID const defaultMaterialID( PhysicsMaterial::DefaultID );
        m_physicsMaterialID = defaultMaterialID;
    }

    bool PhysicsBoxComponent::HasValidPhysicsSetup() const
    {
        if ( m_boxExtents.m_x <= 0.0f || m_boxExtents.m_y <= 0.0f || m_boxExtents.m_z <= 0.0f )
        {
            KRG_LOG_ERROR( "Physics", "Invalid box extents on Physics Box Component: %s (%s). Negative or zero values are not allowed!", GetName().c_str(), GetID().ToString().c_str() );
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