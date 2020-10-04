#ifdef _WIN32
#include "RawMesh.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        bool RawMesh::VertexData::operator==( VertexData const& rhs ) const
        {
            if ( m_position != rhs.m_position )
            {
                return false;
            }

            if ( m_normal != rhs.m_normal )
            {
                return false;
            }

            if ( m_tangent != rhs.m_tangent )
            {
                return false;
            }

            if ( m_texCoords.size() != rhs.m_texCoords.size() )
            {
                return false;
            }

            S32 const numTexCoords = (S32) m_texCoords.size();
            for ( S32 i = 0; i < numTexCoords; i++ )
            {
                if ( m_texCoords[i] != rhs.m_texCoords[i] )
                {
                    return false;
                }
            }

            return true;
        }

        //-------------------------------------------------------------------------

        bool RawMesh::IsValid() const
        {
            if ( HasErrors() )
            {
                return false;
            }

            if ( m_isSkeletalMesh )
            {
                if ( !m_skeleton.IsValid() )
                {
                    return false;
                }
            }

            if ( m_geometrySections.empty() )
            {
                return false;
            }

            return true;
        }
    }
}

#endif
