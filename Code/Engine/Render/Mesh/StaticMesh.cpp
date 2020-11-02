#include "StaticMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool StaticMesh::IsValid() const
    {
        return m_indexBuffer.IsValid() && m_vertexBuffer.IsValid();
    }
}