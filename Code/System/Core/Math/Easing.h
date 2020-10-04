#pragma once

#include "Math.h"

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Easing utility functions 
//  - Adapted from Branimir Karadzic's BX lib ( https://github.com/bkaradzic/bx )
//  - Reference: http://easings.net/ and http://robertpenner.com/easing/
//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        namespace Easing
        {
            enum class Type
            {
                Linear,
                InQuad,
                OutQuad,
                InOutQuad,
                OutInQuad,
                InCubic,
                OutCubic,
                InOutCubic,
                OutInCubic,
                InQuart,
                OutQuart,
                InOutQuart,
                OutInQuart,
                InQuint,
                OutQuint,
                InOutQuint,
                OutInQuint,
                InSine,
                OutSine,
                InOutSine,
                OutInSine,
                InExpo,
                OutExpo,
                InOutExpo,
                OutInExpo,
                InCirc,
                OutCirc,
                InOutCirc,
                OutInCirc,
            };

            typedef F32( *EasingFuncPtr )( F32 t );

            // Helper functions
            //-------------------------------------------------------------------------

            F32 EvaluateEasingFunction( Type type, F32 parameter );

            template<EasingFuncPtr easeFuncPtr>
            F32 EaseIn( F32 t )
            {
                return easeFuncPtr( t );
            }

            template<EasingFuncPtr easeFuncPtr>
            F32 EaseOut( F32 t )
            {
                return 1.0f - easeFuncPtr( 1.0f - t );
            }

            template<EasingFuncPtr easeInFunc, EasingFuncPtr easeOutFunc>
            F32 EaseInOut( F32 t )
            {
                if ( t < 0.5f )
                {
                    return easeInFunc( 2.0f * t ) * 0.5f;
                }
                else
                {
                    return ( easeOutFunc( 2.0f * t - 1.0f ) * 0.5f ) + 0.5f;
                }
            }

            // Individual easing functions
            //-------------------------------------------------------------------------

            inline F32 EaseInLinear( F32 t )
            {
                return t;
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInQuad( F32 t )
            {
                return t * t;
            }

            inline F32 EaseOutQuad( F32 t )
            {
                return EaseOut<EaseInQuad>( t );
            }

            inline F32 EaseInOutQuad( F32 t )
            {
                return EaseInOut<EaseInQuad, EaseOutQuad>( t );
            }

            inline F32 EaseOutInQuad( F32 t )
            {
                return EaseInOut<EaseOutQuad, EaseInQuad>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInCubic( F32 t )
            {
                return t*t*t;
            }

            inline F32 EaseOutCubic( F32 t )
            {
                return EaseOut<EaseInCubic>( t );
            }

            inline F32 EaseInOutCubic( F32 t )
            {
                return EaseInOut<EaseInCubic, EaseOutCubic>( t );
            }

            inline F32 EaseOutInCubic( F32 t )
            {
                return EaseInOut<EaseOutCubic, EaseInCubic>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInQuart( F32 t )
            {
                return t*t*t*t;
            }

            inline F32 EaseOutQuart( F32 t )
            {
                return EaseOut<EaseInQuart>( t );
            }

            inline F32 EaseInOutQuart( F32 t )
            {
                return EaseInOut<EaseInQuart, EaseOutQuart>( t );
            }

            inline F32 EaseOutInQuart( F32 t )
            {
                return EaseInOut<EaseOutQuart, EaseInQuart>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInQuint( F32 t )
            {
                return t*t*t*t*t;
            }

            inline F32 EaseOutQuint( F32 t )
            {
                return EaseOut<EaseInQuint>( t );
            }

            inline F32 EaseInOutQuint( F32 t )
            {
                return EaseInOut<EaseInQuint, EaseOutQuint>( t );
            }

            inline F32 EaseOutInQuint( F32 t )
            {
                return EaseInOut<EaseOutQuint, EaseInQuint>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInSine( F32 t )
            {
                return 1.0f - Cos( t * PiDivTwo );
            }

            inline F32 EaseOutSine( F32 t )
            {
                return EaseOut<EaseInSine>( t );
            }

            inline F32 EaseInOutSine( F32 t )
            {
                return EaseInOut<EaseInSine, EaseOutSine>( t );
            }

            inline F32 EaseOutInSine( F32 t )
            {
                return EaseInOut<EaseOutSine, EaseInSine>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInExpo( F32 t )
            {
                return Pow( 2.0f, 10.0f * ( t - 1.0f ) ) - 0.001f;
            }

            inline F32 EaseOutExpo( F32 t )
            {
                return EaseOut<EaseInExpo>( t );
            }

            inline F32 EaseInOutExpo( F32 t )
            {
                return EaseInOut<EaseInExpo, EaseOutExpo>( t );
            }

            inline F32 EaseOutInExpo( F32 t )
            {
                return EaseInOut<EaseOutExpo, EaseInExpo>( t );
            }

            //-------------------------------------------------------------------------

            inline F32 EaseInCirc( F32 t )
            {
                return -( Sqrt( 1.0f - t*t ) - 1.0f );
            }

            inline F32 EaseOutCirc( F32 t )
            {
                return EaseOut<EaseInCirc>( t );
            }

            inline F32 EaseInOutCirc( F32 t )
            {
                return EaseInOut<EaseInCirc, EaseOutCirc>( t );
            }

            inline F32 EaseOutInCirc( F32 t )
            {
                return EaseInOut<EaseOutCirc, EaseInCirc>( t );
            }
        }
    }
}