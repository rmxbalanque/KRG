#pragma once
#include "Matrix.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct KRG_SYSTEM_CORE_API PointSet
    {

    public:

        PointSet()
            : m_min( FLT_MAX )
            , m_max( -FLT_MAX )
        {}

        void ApplyTransform( Matrix const& transform );
        inline PointSet GetTransformed( Matrix const& transform ) const;

    public:

        Float3              m_min;
        Float3              m_max;
        TVector<Float3>     m_points;
    };

    //-------------------------------------------------------------------------

    inline PointSet PointSet::GetTransformed( Matrix const& transform ) const
    {
        PointSet result = *this;
        result.ApplyTransform( transform );
        return result;
    }
}