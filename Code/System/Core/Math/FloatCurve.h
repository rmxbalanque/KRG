#pragma once
#include "Math.h"
#include "NumericRange.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// Interpolation Curve
//-------------------------------------------------------------------------
// A sequence of piece wise cubic hermite splines -
// This curve is useful for when you want remap one float value to another

namespace KRG
{
    class KRG_SYSTEM_CORE_API FloatCurve
    {
        KRG_SERIALIZE_MEMBERS( m_points );

        #if KRG_DEVELOPMENT_TOOLS
        static uint16 s_pointIdentifierGenerator;
        #endif

    public:

        // Set curve state from a string
        static bool FromString( String const& inStr, FloatCurve& outCurve );

        // The tangent options per point
        enum TangentMode : uint8
        {
            Free,
            Locked,
        };

        // A 1D curve point - the ID is needed for the tooling to distinguish points from one another
        struct Point
        {
            KRG_SERIALIZE_MEMBERS( m_parameter, m_value, m_inTangent, m_outTangent, m_tangentMode );

            inline bool operator==( Point const& rhs ) const
            {
                return m_parameter == rhs.m_parameter && m_value == rhs.m_value && m_inTangent == rhs.m_inTangent && m_outTangent == rhs.m_outTangent && m_tangentMode == rhs.m_tangentMode;
            }

            inline bool operator!=( Point const& rhs ) const { return !operator==( rhs ); }

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

        // Curve query
        //-------------------------------------------------------------------------

        inline int32 const GetNumPoints() const { return (int32) m_points.size(); }
        Point const& GetPoint( int32 pointIdx ) const { KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() ); return m_points[pointIdx]; }

        // Get the range for the parameters that this curve covers - all values outside this range are clamped to the extremity points
        inline FloatRange GetParameterRange() const 
        {
            FloatRange range(0, 0);
            if ( !m_points.empty() )
            {
                range = FloatRange( m_points[0].m_parameter, m_points.back().m_parameter );
            }
            return range;
        }

        // Evaluate the curve and return the value for the specified input parameter
        // If the parameter supplied is outside the parameter range the value returned will be that of the nearest extremity point
        float Evaluate( float parameter );

        // Curve manipulation
        //-------------------------------------------------------------------------

        void AddPoint( float parameter, float value, float inTangent = 1.0f, float outTangent = 1.0f );
        void EditPoint( int32 pointIdx, float parameter, float value );
        void SetPointTangentMode( int32 pointIdx, TangentMode mode );
        void SetPointOutTangent( int32 pointIdx, float tangent );
        void SetPointInTangent( int32 pointIdx, float tangent );
        void RemovePoint( int32 pointIdx );
        void Clear() { m_points.clear(); }

        // Serialization
        //-------------------------------------------------------------------------

        // Set the string state from a string
        // Warning! This will crash on invalid strings, if you are not sure if the string is valid, use the static function supplied
        inline void FromString( String const& inStr ) { bool result = FloatCurve::FromString( inStr, *this ); KRG_ASSERT( result ); }

        // Returns the curve state as a string
        String ToString() const;

        // Operators
        //-------------------------------------------------------------------------

        // Equality operators needed for core types
        bool operator==( FloatCurve const& rhs ) const;

        // Equality operators needed for core types
        inline bool operator!=( FloatCurve const& rhs ) const { return !operator==( rhs ); }

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