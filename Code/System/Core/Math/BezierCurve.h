#pragma once

#include "Base/Math/ComputeTypes/Vector.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace QuadraticBezier
    {
        template<typename T>
        T Evaluate( T p0, T p1, T p2, F32 t )
        {
            auto const a = p0 * ( 1 - t ) * ( 1 - t );
            auto const b = p1 * ( 2 * t ) * ( 1 - t );
            auto const c = p2 * t * t;
            return a + b + c;
        }

        template<typename T>
        T GetTangent( T p0, T p1, T p2, F32 t )
        {
            auto const a = ( p1 - p0 ) * ( 2 * ( 1 - t ) );
            auto const b = ( p2 - p1 ) * 2 * t;
            return a + b;
        }
    }

    //-------------------------------------------------------------------------

    namespace CubicBezier
    {
        template<typename T>
        T Evaluate( T p0, T p1, T p2, T p3, F32 t )
        {
            auto const a = p0 * ( 1 - t ) * ( 1 - t ) * ( 1 - t );
            auto const b = p1 * 3 * t * ( 1 - t ) * ( 1 - t );
            auto const c = p2 * 3 * t * t * ( 1 - t );
            auto const d = p3 * t * t * t;
            return a + b + c + d;
        }

        template<typename T>
        T GetTangent( T p0, T p1, T p2, T p3, F32 t )
        {
            auto const a = p0 * ( -1 + ( 2 * t ) - ( t * t ) );
            auto const b = p1 * ( 1 - ( 4 * t ) + ( 3 * t * t ) );
            auto const c = p2 * ( ( 2 * t ) - ( 3 * t * t ) );
            auto const d = p3 * ( t * t );
            return a + b + c + d;
        }
    }
}