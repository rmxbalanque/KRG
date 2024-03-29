#pragma once
#include "RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API SkeletalMesh : public Mesh
    {
        friend class SkeletalMeshCompiler;
        friend class MeshLoader;

        KRG_REGISTER_RESOURCE( 'SMSH', "Skeletal Mesh" );
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Mesh ), KRG_NVP( m_boneIDs ), KRG_NVP( m_parentBoneIndices ), KRG_NVP( m_bindPose ), KRG_NVP( m_inverseBindPose ) );

    public:

        virtual bool IsValid() const override;

        // Bone Info
        inline int32 GetNumBones() const { return (int32) m_boneIDs.size(); }
        int32 GetBoneIndex( StringID const& boneID ) const;
        inline TVector<int32> const& GetParentBoneIndices() const { return m_parentBoneIndices; }
        inline int32 GetParentBoneIndex( int32 const& idx ) const { KRG_ASSERT( idx < m_parentBoneIndices.size() ); return m_parentBoneIndices[idx]; }
        StringID GetBoneID( int32 idx ) const { KRG_ASSERT( idx < m_boneIDs.size() ); return m_boneIDs[idx]; }

        // Bind Poses
        inline TVector<Transform> const& GetBindPose() const { return m_bindPose; }
        inline TVector<Transform> const& GetInverseBindPose() const { return m_inverseBindPose; }

        // Debug
        #if KRG_DEVELOPMENT_TOOLS
        void DrawBindPose( Drawing::DrawContext& drawingContext, Transform const& worldTransform ) const;
        #endif

    private:

        TVector<StringID>                   m_boneIDs;
        TVector<int32>                      m_parentBoneIndices;
        TVector<Transform>                  m_bindPose;             // Note: bind pose is in global space
        TVector<Transform>                  m_inverseBindPose;
    };
}