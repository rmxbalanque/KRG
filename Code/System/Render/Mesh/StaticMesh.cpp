#include "StaticMesh.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool StaticMesh::IsValid() const
        {
            return m_indexBuffer.IsValid() && m_vertexBuffer.IsValid();
        }
    }
}