#include "RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    Mesh::GeometrySection::GeometrySection( StringID ID, uint32 startIndex, uint32 numIndices )
        : m_ID( ID )
        , m_startIndex( startIndex )
        , m_numIndices( numIndices )
    {}

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Mesh::DrawNormals( Drawing::DrawContext& drawingContext, Transform const& worldTransform ) const
    {
        KRG_UNIMPLEMENTED_FUNCTION();
    }
    #endif
}