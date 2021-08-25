#include "AnimationSkeleton.h"
#include "System/Core/Debug/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    bool Skeleton::IsValid() const
    {
        return !m_boneIDs.empty() && ( m_boneIDs.size() == m_parentIndices.size() ) && ( m_boneIDs.size() == m_localReferencePose.size() );
    }

    Transform Skeleton::GetBoneGlobalTransform( int32 idx ) const
    {
        KRG_ASSERT( idx >= 0 && idx < m_localReferencePose.size() );

        Transform boneGlobalTransform = m_localReferencePose[idx];
        int32 parentIdx = GetParentBoneIndex( idx );

        while ( parentIdx != InvalidIndex )
        {
            boneGlobalTransform = boneGlobalTransform * m_localReferencePose[parentIdx];
            parentIdx = GetParentBoneIndex( parentIdx );
        }

        return boneGlobalTransform;
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Skeleton::DrawDebug( Debug::DrawingContext& ctx, Transform const& worldTransform ) const
    {
        auto const numBones = m_localReferencePose.size();
        if ( numBones > 0 )
        {
            TInlineVector<Transform, 256> globalTransforms;
            globalTransforms.resize( numBones );

            globalTransforms[0] = m_localReferencePose[0] * worldTransform;
            for ( auto i = 1; i < numBones; i++ )
            {
                auto const& parentIdx = m_parentIndices[i];
                auto const& parentTransform = globalTransforms[parentIdx];
                globalTransforms[i] = m_localReferencePose[i] * parentTransform;
            }

            for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
            {
                auto const& parentIdx = m_parentIndices[boneIdx];
                auto const& parentTransform = globalTransforms[parentIdx];
                auto const& boneTransform = globalTransforms[boneIdx];

                ctx.DrawLine( boneTransform.GetTranslation().ToFloat3(), parentTransform.GetTranslation().ToFloat3(), Colors::HotPink, 2.0f );
                ctx.DrawAxis( boneTransform, 0.03f, 3.0f );
            }
        }
    }
    #endif
}