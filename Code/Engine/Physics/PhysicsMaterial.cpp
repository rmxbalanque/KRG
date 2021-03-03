#include "PhysicsMaterial.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsMaterialSettings::IsValid() const
    {
        if ( !m_name.IsValid() )
        {
            return false;
        }

        if ( !Math::IsInRangeInclusive( m_staticFriction, 0.0f, FLT_MAX ) || !Math::IsInRangeInclusive( m_dynamicFriction, 0.0f, FLT_MAX ) )
        {
            return false;
        }

        return Math::IsInRangeInclusive( m_restitution, 0.0f, 1.0f );
    }

    //-------------------------------------------------------------------------

    bool PhysicsMaterial::IsValid() const
    {
        return m_pMaterial != nullptr;
    }
} 