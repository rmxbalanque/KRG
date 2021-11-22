#pragma once
#include "Math.h"
#include "System/Core/Types/Containers.h"
#include "NumericRange.h"

//-------------------------------------------------------------------------
// Interpolation Curve
//-------------------------------------------------------------------------
// A sequence of piece wise cubic hermite splines -
// This curve is useful for when you want remap one float value to another

namespace KRG
{
    class KRG_SYSTEM_CORE_API FloatCurve
    {
        #if KRG_DEVELOPMENT_TOOLS
        static uint16 m_pointIdentifierGenerator;
        #endif

    public:

        enum TangentMode : uint8
        {
            Free,
            Locked,
        };

        struct Point
        {
            float           m_parameter;
            float           m_value;
            float           m_inTangent = 1;
            float           m_outTangent = 1;
            TangentMode     m_tangentMode = TangentMode::Free;

            #if KRG_DEVELOPMENT_TOOLS
            uint16          m_ID;
            #endif
        };

    public:

        inline int32 const GetNumPoints() const { return (int32) m_points.size(); }

        inline FloatRange GetParameterRange() const 
        {
            FloatRange range(0, 0);
            if ( !m_points.empty() )
            {
                range = FloatRange( m_points[0].m_parameter, m_points.back().m_parameter );
            }
            return range;
        }

        float Evaluate( float parameter );

        Point const& GetPoint( int32 pointIdx ) const { KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() ); return m_points[pointIdx]; }

        void AddPoint( float parameter, float value );
        void EditPoint( int32 pointIdx, float parameter, float value );
        void SetPointTangentMode( int32 pointIdx, TangentMode mode );
        void SetPointOutTangent( int32 pointIdx, float tangent );
        void SetPointInTangent( int32 pointIdx, float tangent );
        void RemovePoint( int32 pointIdx );
        void Clear() { m_points.clear(); }

    private:

        inline void SortPoints()
        {
            auto SortPredicate = [] ( Point const& a, Point const& b )
            {
                return a.m_parameter < b.m_parameter;
            };

            eastl::sort( m_points.begin(), m_points.end(), SortPredicate );
        }

    private:

        TInlineVector<Point, 8>     m_points; // Space for 4 curves
    };
}