#ifdef _WIN32
#pragma once

#include "RawSkeleton.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        class KRG_TOOLS_RESOURCE_API RawMesh : public RawAsset
        {

        public:

            struct VertexData
            {
                VertexData() = default;

                bool operator==( VertexData const& rhs ) const;

                inline bool operator!=( VertexData const& rhs ) const
                {
                    return !( *this == rhs );
                }

            public:

                Float4                              m_position = Float4::Zero;
                Float4                              m_normal = Float4::Zero;
                Float4                              m_tangent = Float4::Zero;
                TInlineVector<Float2, 3>            m_texCoords;

                // Optional skinning data
                TVector<S32>                        m_boneIndices;
                TVector<F32>                        m_boneWeights;
            };

            //-------------------------------------------------------------------------

            struct GeometrySection
            {
                GeometrySection() = default;

                inline U32 GetNumTriangles() const { return (U32) m_indices.size() / 3; }

            public:

                String                              m_name;
                TVector<VertexData>                 m_vertices;
                TVector<U32>                        m_indices;

                bool                                m_clockwiseWinding = false;
            };

        public:

            RawMesh() = default;
            virtual bool IsValid() const override final;

            inline S32 GetNumGeometrySections() const { return (S32) m_geometrySections.size(); }
            inline TVector<GeometrySection> const& GetGeometrySections() const { return m_geometrySections; }

            inline S32 GetNumUVChannels() const { return m_numUVChannels; }

            inline bool IsSkeletalMesh() const { return m_isSkeletalMesh; }
            inline RawSkeleton const& GetSkeleton() const { KRG_ASSERT( IsSkeletalMesh() ); return m_skeleton; }
            inline S32 GetNumBones() const { KRG_ASSERT( IsSkeletalMesh() ); return m_skeleton.GetNumBones(); }
            inline S32 GetNumBoneInfluencesPerVertex() const { KRG_ASSERT( IsSkeletalMesh() ); return m_maxNumberOfBoneInfluences; }

        protected:

            TVector<GeometrySection>            m_geometrySections;
            TVector<String>                     m_warnings;
            TVector<String>                     m_errors;

            RawSkeleton                         m_skeleton;
            S32                                 m_maxNumberOfBoneInfluences = 0;
            S32                                 m_numUVChannels = 0;
            bool                                m_isSkeletalMesh = false;
        };
    }
}

#endif
