#include "SkeletalMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void SkeletalMeshComponent::Initialize()
        {
            SpatialEntityComponent::Initialize();

            KRG_ASSERT( m_pMesh != nullptr && m_pMesh.IsLoaded() );
            SetLocalBounds( m_pMesh->GetBounds() );

            // Initialize to the mesh bind pose
            m_boneTransforms = m_pMesh->GetBindPose();
            m_skinningTransforms.resize( m_boneTransforms.size() );
            FinalizePose();
        }

        void SkeletalMeshComponent::Shutdown()
        {
            m_boneTransforms.clear();
            m_skinningTransforms.clear();

            SpatialEntityComponent::Shutdown();
        }

        bool SkeletalMeshComponent::TryFindAttachmentSocketTransform( StringID socketID, Transform& outSocketWorldTransform ) const
        {
            KRG_ASSERT( socketID.IsValid() );

            outSocketWorldTransform = GetWorldTransform();
           
            if ( m_pMesh.IsLoaded() )
            {
                auto const boneIdx = m_pMesh->GetBoneIndex( socketID );
                if ( boneIdx != InvalidIndex )
                {
                    if ( IsInitialized() )
                    {
                        outSocketWorldTransform = m_boneTransforms[boneIdx] * outSocketWorldTransform;
                    }
                    else
                    {
                        outSocketWorldTransform = m_pMesh->GetBindPose()[boneIdx] * outSocketWorldTransform;
                    }

                    return true;
                }
            }

            return false;
        }

        bool SkeletalMeshComponent::HasSocket( StringID socketID ) const
        {
            KRG_ASSERT( socketID.IsValid() );

            if( m_pMesh.IsLoaded() )
            {
                int32 boneIdx = m_pMesh->GetBoneIndex( socketID );
                return boneIdx != InvalidIndex;
            }
            
            return false;
        }

        //-------------------------------------------------------------------------

        void SkeletalMeshComponent::FinalizePose()
        {
            NotifySocketsUpdated();
            UpdateBounds();
            UpdateSkinningTransforms();
        }

        //-------------------------------------------------------------------------

        void SkeletalMeshComponent::UpdateBounds()
        {
            KRG_ASSERT( m_pMesh != nullptr );

            AABB newBounds;
            for ( auto const& boneTransform : m_boneTransforms )
            {
                newBounds.AddPoint( boneTransform.GetTranslation() );
            }

            SetLocalBounds( OBB( newBounds ) );
        }

        void SkeletalMeshComponent::UpdateSkinningTransforms()
        {
            auto const numBones = m_boneTransforms.size();
            KRG_ASSERT( m_skinningTransforms.size() == numBones );

            auto const& inverseBindPose = m_pMesh->GetInverseBindPose();
            for ( auto i = 0; i < numBones; i++ )
            {
                Transform const skinningTransform = inverseBindPose[i] * m_boneTransforms[i];
                m_skinningTransforms[i] = ( skinningTransform ).ToMatrix();
            }
        }
    }
}
