#pragma once

#include "MeshComponent.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API SkeletalMeshComponent : public MeshComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( SkeletalMeshComponent );

    public:

        using MeshComponent::MeshComponent;

        // Mesh Data
        //-------------------------------------------------------------------------

        inline void SetMesh( ResourceID meshResourceID )
        {
            KRG_ASSERT( IsUnloaded() );
            KRG_ASSERT( meshResourceID.IsValid() );
            m_pMesh = meshResourceID;
        }

        inline SkeletalMesh const* GetMesh() const
        {
            KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
            return m_pMesh.GetPtr();
        }

        // Skeletal Pose
        //-------------------------------------------------------------------------

        // Get the character space (global) transforms for the mesh
        inline TVector<Transform> const& GetBoneTransforms() const { return m_boneTransforms; }

        // The the global space transform for a specific bone
        inline void SetBoneTransform( int32 boneIdx, Transform const& transform )
        {
            KRG_ASSERT( boneIdx >= 0 && boneIdx < m_boneTransforms.size() );
            m_boneTransforms[boneIdx] = transform;
        }

        // This function will finalize the pose, run any procedural bone solvers and generate the skinning transforms
        // Only run this function once per frame once you have set the final global pose
        void FinalizePose();

        // Get the skinning transforms for this mesh - these are the global transforms relative to the bind pose
        inline TVector<Matrix> const& GetSkinningTransforms() const { return m_skinningTransforms; }

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void DrawPose( Debug::DrawingContext& drawingContext ) const;
        #endif

    protected:

        virtual TVector<TResourcePtr<Render::Material>> const& GetDefaultMaterials() const override final;

        void UpdateBounds();
        void UpdateSkinningTransforms();

        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual bool TryFindAttachmentSocketTransform( StringID socketID, Transform& outSocketWorldTransform ) const override final;
        virtual bool HasSocket( StringID socketID ) const override final;

    protected:

        KRG_EXPOSE TResourcePtr<SkeletalMesh>           m_pMesh;
        TVector<Transform>                          m_boneTransforms;
        TVector<Matrix>                             m_skinningTransforms;
    };
}