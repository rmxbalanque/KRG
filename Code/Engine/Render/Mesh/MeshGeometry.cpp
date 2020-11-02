#include "MeshGeometry.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    MeshGeometry::GeometrySection::GeometrySection( U32 startIndex, U32 numIndices )
        : m_startIndex( startIndex )
        , m_numIndices( numIndices )
    {}
}