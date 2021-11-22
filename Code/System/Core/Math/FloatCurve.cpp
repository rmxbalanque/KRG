#include "FloatCurve.h"
#include "Curves.h"
#include "Vector.h"

//-------------------------------------------------------------------------

namespace KRG
{
    #if KRG_DEVELOPMENT_TOOLS
    // ID generator needed for curve editor
    uint16 FloatCurve::m_pointIdentifierGenerator = 0;
    #endif

    //-------------------------------------------------------------------------

    float FloatCurve::Evaluate( float parameter )
    {
        float result = 0;

        if ( m_points.empty() )
        {
            return result;
        }

        if ( m_points.size() == 1 )
        {
            return m_points[0].m_value;
        }

        //-------------------------------------------------------------------------

        FloatRange const parameterRange = GetParameterRange();

        if ( parameterRange.ContainsExclusive( parameter ) )
        {
            int32 const numPoints = GetNumPoints();
            int32 const numCurves = numPoints - 1;
            for ( int32 i = 0; i < numCurves; i++ )
            {
                int32 const startIdx = i;
                int32 const endIdx = i + 1;

                // If the parameter is within this curve, evaluate it
                if ( parameter >= m_points[startIdx].m_parameter && parameter <= m_points[endIdx].m_parameter )
                {
                    float const T = ( parameter - m_points[startIdx].m_parameter ) / ( m_points[endIdx].m_parameter - m_points[startIdx].m_parameter );
                    result = Math::CubicHermite::Evaluate( m_points[startIdx].m_value, m_points[startIdx].m_outTangent, m_points[endIdx].m_value, m_points[endIdx].m_inTangent, T );
                    break;
                }
            }
        }
        else // Outside curve range
        {
            if ( parameter <= parameterRange.m_start )
            {
                result = m_points[0].m_value;
            }
            else
            {
                result = m_points.back().m_value;
            }
        }

        return result;
    }

    void FloatCurve::AddPoint( float parameter, float value )
    {
        #if KRG_DEVELOPMENT_TOOLS
        static uint16 s_pointIdentifierGenerator;
        #endif

        m_points.push_back( { parameter, value } );

        #if KRG_DEVELOPMENT_TOOLS
        // We need a unique ID for the curve editor
        m_points.back().m_ID = ++s_pointIdentifierGenerator;
        #endif

        SortPoints();
    }

    void FloatCurve::EditPoint( int32 pointIdx, float parameter, float value )
    {
        KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() );
        m_points[pointIdx].m_parameter = parameter;
        m_points[pointIdx].m_value = value;
        SortPoints();
    }

    void FloatCurve::SetPointTangentMode( int32 pointIdx, TangentMode mode )
    {
        KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() );
        m_points[pointIdx].m_tangentMode = mode;
    }

    void FloatCurve::SetPointOutTangent( int32 pointIdx, float tangent )
    {
        KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() );
        m_points[pointIdx].m_outTangent = tangent;
    }

    void FloatCurve::SetPointInTangent( int32 pointIdx, float tangent )
    {
        m_points[pointIdx].m_inTangent = tangent;
    }

    void FloatCurve::RemovePoint( int32 pointIdx )
    {
        KRG_ASSERT( pointIdx >= 0 && pointIdx < GetNumPoints() );

        m_points.erase( m_points.begin() + pointIdx );
    }
}