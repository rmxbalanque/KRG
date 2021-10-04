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
        InlineString<64> s;
        s.sprintf( "x=%.3f, y=%.3f, z=%.3f", vector.m_x, vector.m_y, vector.m_z );
        return s;
    }

    inline InlineString<64> ToString( Quaternion q )
    {
        InlineString<64> s;
        EulerAngles angles = q.ToEulerAngles();
        s.sprintf( "x=%.3f, y=%.3f, z=%.3f", angles.m_x, angles.m_y, angles.m_z );
        return s;
    }
}