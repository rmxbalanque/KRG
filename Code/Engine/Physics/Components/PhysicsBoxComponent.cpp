#include "PhysicsBoxComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsBoxComponent::HasValidPhysicsSetup() const
    {
        if ( m_boxExtents.m_x <= 0.0f || m_boxExtents.m_y <= 0.0f || m_boxExtents.m_z <= 0.0f )
        {
            KRG_LOG_ERROR( "Invalid box extents on Physics Box Component: %s (%s). Negative or zero values are not allowed!", GetName().c_str(), GetID().ToString().c_str() );
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