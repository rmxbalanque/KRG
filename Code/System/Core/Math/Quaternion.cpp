#include "Quaternion.h"
#include "Matrix.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Quaternion const Quaternion::Identity( 0, 0, 0, 1 );

    //-------------------------------------------------------------------------

    // KRG Rotation order is XYZ
    EulerAngles Quaternion::ToEulerAngles() const
    {
        return Matrix( *this ).ToEulerAngles();
    }
}