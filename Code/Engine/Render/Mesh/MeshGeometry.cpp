#include "MeshGeometry.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    MeshGeometry::GeometrySection::GeometrySection( uint32 startIndex, uint32 numIndices )
        : m_startIndex( startIndex )
        , m_numIndices( numIndices )
    {}
}