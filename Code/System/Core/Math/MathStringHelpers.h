#pragma once
#include "Math.h"
#include "Vector.h"
#include "System/Core/Types/String.h"
#include "Quaternion.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    KRG_SYSTEM_CORE_API char const* ToString( Axis axis );

    //-------------------------------------------------------------------------

    inline InlineString<64> ToString( Vector vector )
    {
        return InlineString<64>( InlineString<64>::CtorSprintf(), "x=%.3f, y=%.3f, z=%.3f", vector.m_x, vector.m_y, vector.m_z );
    }

    inline InlineString<64> ToString( Quaternion q )
    {
        EulerAngles const angles = q.ToEulerAngles();
        return InlineString<64>( InlineString<64>::CtorSprintf(), "x=%.3f, y=%.3f, z=%.3f", angles.m_x, angles.m_y, angles.m_z );
    }
}