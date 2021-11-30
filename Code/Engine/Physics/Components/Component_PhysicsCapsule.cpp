#include "Component_PhysicsCapsule.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    // This constructor only exists to lazy initialize the static default material ID
    CapsuleComponent::CapsuleComponent()
    {
        static StringID const defaultMaterialID( PhysicsMaterial::DefaultID );
        m_physicsMaterialID = defaultMaterialID;
        SetLocalTransform( Transform( Quaternion( AxisAngle( Vector::UnitY, Math::PiDivTwo ) ) ) );
    }

    bool CapsuleComponent::HasValidPhysicsSetup() const
    {
        if ( m_radius <= 0 || m_cylinderPortionHalfHeight <= 0 )
        {
            KRG_LOG_ERROR( "Physics", "Invalid radius or half height on Physics Capsule Component: %s (%u). Negative or zero values are not allowed!", GetName().c_str(), GetID() );
            return false;
        }

        if ( !m_physicsMaterialID.IsValid() )
        {
            KRG_LOG_ERROR( "Physics", "Invalid physical material setup on Physics Component: %s (%u)", GetName().c_str() );
            return false;
        }

        return true;
    }
}