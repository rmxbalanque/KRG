#pragma once
#include "System/Core/_Module/API.h"
#include "System/Core/Math/Vector.h"
#include "Range.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        class Rectangle
        {

        public:

            Rectangle() = default;

            explicit Rectangle( Float2 topLeft, Float2 size )
                : m_topLeft( topLeft )
                , m_size( size )
            {
                KRG_ASSERT( size.x >= 0 && size.y >= 0 );
            }

            inline Float2 GetTopLeft() const { return m_topLeft; }
            inline Float2 GetBottomRight() const { return m_topLeft + Float2( m_size.x, -m_size.y ); }
            inline Float2 GetSize() const { return m_size; }

            inline F32 GetArea() const
            {
               return m_size.x * m_size.y;
            }

            inline bool IsDegenerate() const
            {
                return Math::IsNearlyZero( GetArea() );
            }

            inline Float2 GetCenter() const
            {
                return Float2( m_topLeft.x + ( m_size. x / 2.0f ), m_topLeft.y + ( m_size.x / 2.0f ) );
            }

            inline Float2 GetClosestPoint( Float2 const& point )
            {
                Float2 const bottomRight = GetBottomRight();

                Float2 closestPoint;
                closestPoint.x = Math::Clamp( point.x, m_topLeft.x, bottomRight.x );
                closestPoint.y = Math::Clamp( point.y, bottomRight.y, m_topLeft.y);
                return closestPoint;
            }

            inline bool Contains( Float2 const& point ) const
            {
                Float2 const bottomRight = GetBottomRight();
                return Math::IsInRange( point.x, m_topLeft.x, bottomRight.x ) && Math::IsInRange( point.y, bottomRight.y, m_topLeft.y );
            }

            inline bool Overlaps( Rectangle const& other ) const
            {
                Float2 const bottomRight = GetBottomRight();
                TRange<F32> rangeX( m_topLeft.x, bottomRight.x );
                TRange<F32> rangeY( bottomRight.y, m_topLeft.y );

                Float2 const otherBottomRight = other.GetBottomRight();
                TRange<F32> otherRangeX( other.m_topLeft.x, otherBottomRight.x );
                TRange<F32> otherRangeY( otherBottomRight.y, other.m_topLeft.y );

                return rangeX.Overlaps( otherRangeX ) && rangeY.Overlaps( otherRangeY );
            }

        private:

            Float2      m_topLeft;
            Float2      m_size;
        };
    }
}