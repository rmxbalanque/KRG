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

    inline InlineString<128> ToString( Vector vector )
    {
        return InlineString<128>( InlineString<128>::CtorSprintf(), "x=%.3f, y=%.3f, z=%.3f", vector.m_x, vector.m_y, vector.m_z );
    }

    inline InlineString<128> ToString( Quaternion q )
    {
        EulerAngles const angles = q.ToEulerAngles();
        Float3 const anglesInDegrees = angles.GetAsDegrees();
        return InlineString<128>( InlineString<128>::CtorSprintf(), "x=%.3f, y=%.3f, z=%.3f", anglesInDegrees.m_x, anglesInDegrees.m_y, anglesInDegrees.m_z );
    }
}