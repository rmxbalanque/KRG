#ifdef _WIN32
#include "FbxSkeleton.h"
#include "FbxSceneContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        class FbxRawSkeleton : public RawSkeleton
        {
            friend class FbxSkeletonFileReader;
        };

        //-------------------------------------------------------------------------

        class FbxSkeletonFileReader
        {
        public:

            static TUniquePtr<KRG::RawAssets::RawSkeleton> ReadSkeleton( FileSystemPath const& sourceFilePath, String const& skeletonRootBoneName )
            {
                KRG_ASSERT( sourceFilePath.IsValid() );

                TUniquePtr<RawSkeleton> pSkeleton( KRG::New<RawSkeleton>() );

                FbxRawSkeleton* pRawSkeleton = (FbxRawSkeleton*) pSkeleton.get();

                //-------------------------------------------------------------------------

                Fbx::FbxSceneContext sceneCtx( sourceFilePath );
                if ( sceneCtx.IsValid() )
                {
                    ReadSkeleton( sceneCtx, skeletonRootBoneName, *pRawSkeleton );
                }
                else
                {
                    pRawSkeleton->LogError( "Failed to read FBX file: %s -> %s", sourceFilePath.c_str(), sceneCtx.GetErrorMessage().c_str() );
                }

                return pSkeleton;
            }

            static void ReadSkeleton( Fbx::FbxSceneContext const& sceneCtx, String const& skeletonRootBoneName, FbxRawSkeleton& rawSkeleton )
            {
                TVector<FbxNode*> skeletonRootNodes;
                sceneCtx.FindAllNodesOfType( FbxNodeAttribute::eSkeleton, skeletonRootNodes );

                auto const numSkeletons = skeletonRootNodes.size();
                if ( numSkeletons == 0 )
                {
                    rawSkeleton.LogError( "No Skeletons found in FBX scene" );
                    return;
                }

                FbxNode* pSkeletonToUse = nullptr;
                if ( !skeletonRootBoneName.empty() )
                {
                    for ( auto& pSkeletonNode : skeletonRootNodes )
                    {
                        if ( pSkeletonNode->GetNameWithoutNameSpacePrefix() == skeletonRootBoneName )
                        {
                            pSkeletonToUse = pSkeletonNode;
                            break;
                        }
                    }

                    if ( pSkeletonToUse == nullptr )
                    {
                        rawSkeleton.LogError( "Couldn't find specified skeleton root: %s", skeletonRootBoneName.c_str() );
                        return;
                    }
                }
                else
                {
                    pSkeletonToUse = skeletonRootNodes[0];
                }

                //-------------------------------------------------------------------------

                rawSkeleton.m_name = StringID( (char const*) pSkeletonToUse->GetNameWithoutNameSpacePrefix() );

                ReadBoneHierarchy( rawSkeleton, sceneCtx, pSkeletonToUse, -1 );
                rawSkeleton.CalculateGlobalTransforms();
            }

            static void ReadBoneHierarchy( FbxRawSkeleton& rawSkeleton, Fbx::FbxSceneContext const& sceneCtx, fbxsdk::FbxNode* pNode, S32 parentIdx )
            {
                KRG_ASSERT( pNode != nullptr && pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton );

                auto const boneIdx = (S32) rawSkeleton.m_bones.size();
                rawSkeleton.m_bones.push_back( RawSkeleton::BoneData( (char const*) pNode->GetNameWithoutNameSpacePrefix() ) );
                rawSkeleton.m_bones[boneIdx].m_parentBoneIdx = parentIdx;

                // Read Bone transform
                // Note: We need to apply the scale correction manually to the translation value, since the scene conversion doesnt modify the local transforms
                FbxAMatrix nodeLocalTransform = pNode->EvaluateLocalTransform();
                nodeLocalTransform.SetT( nodeLocalTransform.GetT() * sceneCtx.GetScaleConversionMultiplier() );
                rawSkeleton.m_bones[boneIdx].m_localTransform = sceneCtx.ConvertMatrixToTransform( nodeLocalTransform );

                // Read child bones
                auto const numChildren = pNode->GetChildCount();
                for ( int i = 0; i < numChildren; i++ )
                {
                    FbxNode* pChildNode = pNode->GetChild( i );
                    auto const attributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
                    if ( attributeType == FbxNodeAttribute::eSkeleton )
                    {
                        ReadBoneHierarchy( rawSkeleton, sceneCtx, pChildNode, boneIdx );
                    }
                }
            }
        };
    }

    //-------------------------------------------------------------------------

    TUniquePtr<KRG::RawAssets::RawSkeleton> Fbx::ReadSkeleton( FileSystemPath const& sourceFilePath, String const& skeletonRootBoneName )
    {
        return RawAssets::FbxSkeletonFileReader::ReadSkeleton( sourceFilePath, skeletonRootBoneName );
    }

    void Fbx::ReadSkeleton( FbxSceneContext const& sceneCtx, String const& skeletonRootBoneName, RawAssets::RawSkeleton& skeleton )
    {
        return RawAssets::FbxSkeletonFileReader::ReadSkeleton( sceneCtx, skeletonRootBoneName, (RawAssets::FbxRawSkeleton&) skeleton );
    }
}
#endif