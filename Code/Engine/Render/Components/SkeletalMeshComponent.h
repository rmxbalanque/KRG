#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Render/Mesh/SkeletalMesh.h"
#include "System/Render/RenderMaterial.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_ENGINE_RENDER_API SkeletalMeshComponent : public SpatialEntityComponent
        {
            KRG_REGISTER_ENTITY_COMPONENT;

        public:

            // Mesh Data
            //-------------------------------------------------------------------------

            inline SkeletalMesh const* GetMesh() const
            {
                KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
                return m_pMesh.GetPtr();
            }

            inline TVector<TResourcePtr<Material>> const& GetMaterials() const
            {
                KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
                return m_materials;
            }

            // Skeletal Pose
            //-------------------------------------------------------------------------

            // Get the character space (global) transforms for the mesh
            inline TVector<Transform> const& GetBoneTransforms() const { return m_boneTransforms; }

            // The the global space transform for a specific bone
            inline void SetBoneTransform( S32 boneIdx, Transform const& transform )
            {
                KRG_ASSERT( boneIdx >= 0 && boneIdx < m_boneTransforms.size() );
                m_boneTransforms[boneIdx] = transform;
            }

            // This function will finalize the pose, run any procedural bone solvers and generate the skinning transforms
            // Only run this function once per frame once you have set the final global pose
            void FinalizePose();

            // Get the skinning transforms for this mesh - these are the global transforms relative to the bind pose
            inline TVector<Matrix> const& GetSkinningTransforms() const { return m_skinningTransforms; }

        protected:

            void UpdateBounds();
            void UpdateSkinningTransforms();

            virtual void Initialize() override;
            virtual void Shutdown() override;

            virtual bool TryFindAttachmentSocketTransform( StringID socketID, Transform& outSocketWorldTransform ) const override final;
            virtual bool HasSocket( StringID socketID ) const override final;

        protected:

            EXPOSE TResourcePtr<SkeletalMesh>           m_pMesh;
            EXPOSE TVector<TResourcePtr<Material>>      m_materials;
            TVector<Transform>                          m_boneTransforms;
            TVector<Matrix>                             m_skinningTransforms;
        };
    }
}