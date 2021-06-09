#include "SkeletalMeshComponent.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void SkeletalMeshComponent::Initialize()
    {
        MeshComponent::Initialize();

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

        MeshComponent::Shutdown();
    }

    TVector<TResourcePtr<Render::Material>> const& SkeletalMeshComponent::GetDefaultMaterials() const
    {
        KRG_ASSERT( IsInitialized() );
        return m_pMesh->GetMaterials();
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

        if ( m_pMesh.IsLoaded() )
        {
            int32 boneIdx = m_pMesh->GetBoneIndex( socketID );
            return boneIdx != InvalidIndex;
        }

        return false;
    }

    //-------------------------------------------------------------------------

    void SkeletalMeshComponent::FinalizePose()
    {
        KRG_PROFILE_FUNCTION_RENDER();

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

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void SkeletalMeshComponent::DrawPose( Debug::DrawingContext& drawingContext ) const
    {
        KRG_ASSERT( IsInitialized() );

        Transform const& worldTransform = GetWorldTransform();
        auto const numBones = m_boneTransforms.size();

        Transform boneWorldTransform = m_boneTransforms[0] * worldTransform;
        drawingContext.DrawBox( boneWorldTransform, Float3( 0.005f ), Colors::Orange );
        drawingContext.DrawAxis( boneWorldTransform, 0.05f );

        for ( auto i = 1; i < numBones; i++ )
        {
            boneWorldTransform = m_boneTransforms[i] * worldTransform;

            auto const parentBoneIdx = m_pMesh->GetBoneParentIndex( i );
            Transform const parentBoneWorldTransform = m_boneTransforms[parentBoneIdx] * worldTransform;

            drawingContext.DrawLine( parentBoneWorldTransform.GetTranslation(), boneWorldTransform.GetTranslation(), Colors::Orange );
            //drawingContext.DrawBox( boneWorldTransform, Float3( 0.003f ), Colors::Orange );
            drawingContext.DrawAxis( boneWorldTransform, 0.03f, 2.0f );

            //StringID const& BoneID = GetBoneID( i );
            //drawingContext.DrawText3D( boneWorldTransform.GetTranslation(), BoneID.ToString(), Colors::Orange, Debug::Drawing::TextSize::Small );
        }
    }
    #endif
}
