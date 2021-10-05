#include "FbxMesh.h"
#include "FbxSceneContext.h"
#include "FbxSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        class FbxRawSkeleton : public RawSkeleton
        {
            friend class FbxMeshFileReader;
        };

        class FbxRawMesh : public RawMesh
        {
            friend class FbxMeshFileReader;
        };

        //-------------------------------------------------------------------------

        class FbxMeshFileReader
        {
        public:

            static TUniquePtr<RawMesh> ReadStaticMesh( FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile )
            {
                KRG_ASSERT( sourceFilePath.IsValid() );

                TUniquePtr<RawMesh> pMesh( KRG::New<RawMesh>() );
                FbxRawMesh* pRawMesh = (FbxRawMesh*) pMesh.get();

                //-------------------------------------------------------------------------

                Fbx::FbxSceneContext sceneCtx( sourceFilePath );
                if ( sceneCtx.IsValid() )
                {
                    ReadAllSubmeshes( *pRawMesh, sceneCtx, nameOfMeshToCompile );
                }
                else
                {
                    pRawMesh->LogError( "Failed to read FBX file: %s -> %s", sourceFilePath.c_str(), sceneCtx.GetErrorMessage().c_str() );
                }

                return pMesh;
            }

            static TUniquePtr<RawMesh> ReadSkeletalMesh( FileSystem::Path const& sourceFilePath, int32 maxBoneInfluences = 4 )
            {
                KRG_ASSERT( sourceFilePath.IsValid() );

                TUniquePtr<RawMesh> pMesh( KRG::New<RawMesh>() );
                FbxRawMesh* pRawMesh = (FbxRawMesh*) pMesh.get();

                //-------------------------------------------------------------------------

                Fbx::FbxSceneContext sceneCtx( sourceFilePath );
                if ( sceneCtx.IsValid() )
                {
                    pRawMesh->m_maxNumberOfBoneInfluences = maxBoneInfluences;
                    pRawMesh->m_isSkeletalMesh = true;

                    // Read mesh data
                    //-------------------------------------------------------------------------

                    ReadAllSubmeshes( *pRawMesh, sceneCtx );

                    if ( pRawMesh->HasErrors() )
                    {
                        return pMesh;
                    }

                    // Since the bind pose is in global space, calculate the local space transforms for the skeleton
                    FbxRawSkeleton& rawSkeleton = (FbxRawSkeleton&) pRawMesh->m_skeleton;
                    rawSkeleton.CalculateLocalTransforms();
                }
                else
                {
                    pRawMesh->LogError( "Failed to read FBX file: %s -> %s", sourceFilePath.c_str(), sceneCtx.GetErrorMessage().c_str() );
                }

                return pMesh;
            }

            //-------------------------------------------------------------------------

            static void ReadAllSubmeshes( FbxRawMesh& rawMesh, Fbx::FbxSceneContext const& sceneCtx, String const& nameOfMeshToCompile = String() )
            {
                FbxGeometryConverter geomConverter( sceneCtx.m_pManager );
                geomConverter.SplitMeshesPerMaterial( sceneCtx.m_pScene, true );

                // Find all meshes in the sceneCtx
                //-------------------------------------------------------------------------

                TInlineVector<FbxMesh*, 20> meshes;
                auto numGeometries = sceneCtx.m_pScene->GetGeometryCount();

                if ( numGeometries == 0 )
                {
                    rawMesh.m_errors.push_back( "No meshes present in the sceneCtx" );
                    return;
                }

                for ( auto i = 0; i < numGeometries; i++ )
                {
                    auto pGeometry = sceneCtx.m_pScene->GetGeometry( i );
                    if ( pGeometry->Is<FbxMesh>() )
                    {
                        FbxMesh* pMesh = static_cast<FbxMesh*>( pGeometry );
                        meshes.push_back( pMesh );
                    }
                }

                // For each mesh found perform necessary corrections and read mesh data
                // Note: this needs to be done in two passes since these operations reorder the geometries in the sceneCtx and pScene->GetGeometry( x ) doesnt return what you expect
                bool meshFound = false;
                TVector<TVector<uint32>> controlPointVertexMapping;
                for ( auto pMesh : meshes )
                {
                    // If we've specified a specific mesh name to compile, skip all other meshes
                    if ( !nameOfMeshToCompile.empty() )
                    {
                        if ( pMesh->GetNode()->GetNameWithoutNameSpacePrefix() != nameOfMeshToCompile )
                        {
                            continue;
                        }
                    }

                    //-------------------------------------------------------------------------

                    meshFound = true;

                    pMesh->RemoveBadPolygons();

                    // Ensure that the mesh is triangulated
                    if ( !pMesh->IsTriangleMesh() )
                    {
                        pMesh = ( fbxsdk::FbxMesh* ) geomConverter.Triangulate( pMesh, true );
                        KRG_ASSERT( pMesh != nullptr && pMesh->IsTriangleMesh() );
                    }

                    // Generate normals if they're not available or not in the right format
                    bool const hasNormals = pMesh->GetElementNormalCount() > 0 && pMesh->GetElementNormal( 0 )->GetMappingMode() != FbxLayerElement::eByPolygonVertex;
                    if ( !hasNormals )
                    {
                        if ( !pMesh->GenerateNormals( true ) )
                        {
                            rawMesh.m_errors.push_back( String().sprintf( "Failed to regenerate mesh normals for mesh: %s", (char const*) pMesh->GetNameWithNameSpacePrefix() ) );
                            return;
                        }
                    }

                    // Allocate space for the control point mapping
                    controlPointVertexMapping.clear();
                    controlPointVertexMapping.resize( pMesh->GetControlPointsCount() );

                    // Create new geometry section
                    RawMesh::GeometrySection& meshData = rawMesh.m_geometrySections.emplace_back( RawMesh::GeometrySection() );
                    meshData.m_name = (char const*) pMesh->GetNode()->GetNameWithNameSpacePrefix();
                    meshData.m_numUVChannels = pMesh->GetElementUVCount();

                    if ( !ReadMeshData( sceneCtx, pMesh, meshData, controlPointVertexMapping ) )
                    {
                        return;
                    }

                    if ( rawMesh.m_isSkeletalMesh )
                    {
                        if ( !ReadSkinningData( rawMesh, sceneCtx, pMesh, meshData, controlPointVertexMapping ) )
                        {
                            return;
                        }
                    }
                }

                if ( !meshFound )
                {
                    rawMesh.m_errors.push_back( String().sprintf( "Couldn't find specified mesh: %s", nameOfMeshToCompile.c_str() ) );
                }
            }

            static bool ReadMeshData( Fbx::FbxSceneContext const& sceneCtx, fbxsdk::FbxMesh* pMesh, FbxRawMesh::GeometrySection& geometryData, TVector<TVector<uint32>>& controlPointVertexMapping )
            {
                KRG_ASSERT( pMesh != nullptr && pMesh->IsTriangleMesh() );

                FbxAMatrix const meshNodeGlobalTransform = sceneCtx.GetNodeGlobalTransform( pMesh->GetNode() );

                //-------------------------------------------------------------------------

                // Check winding order - TODO: actually figure out how to get the real value, right now we assume CCW
                geometryData.m_clockwiseWinding = false;
                FbxVector4 const meshScale = meshNodeGlobalTransform.GetS();
                uint32 const numNegativeAxes = ( ( meshScale[0] < 0 ) ? 1 : 0 ) + ( ( meshScale[1] < 0 ) ? 1 : 0 ) + ( ( meshScale[2] < 0 ) ? 1 : 0 );
                if ( numNegativeAxes == 1 || numNegativeAxes == 3 )
                {
                    geometryData.m_clockwiseWinding = !geometryData.m_clockwiseWinding;
                }

                // Reserve memory for mesh data
                int32 const numPolygons = pMesh->GetPolygonCount();
                int32 const numVertices = numPolygons * 3;
                geometryData.m_vertices.reserve( numVertices );

                for ( auto polygonIdx = 0; polygonIdx < numPolygons; polygonIdx++ )
                {
                    for ( auto vertexIdx = 0; vertexIdx < 3; vertexIdx++ )
                    {
                        RawMesh::VertexData vert;

                        // Get vertex position
                        //-------------------------------------------------------------------------

                        int32 const ctrlPointIdx = pMesh->GetPolygonVertex( polygonIdx, vertexIdx );
                        FbxVector4 const meshVertex = meshNodeGlobalTransform.MultT( pMesh->GetControlPoints()[ctrlPointIdx] );
                        vert.m_position = Vector( (float) meshVertex[0], (float) meshVertex[1], (float) meshVertex[2], (float) meshVertex[3] );
                        vert.m_position.m_w = 1.0f;

                        // Get vertex normal
                        //-------------------------------------------------------------------------

                        FbxVector4 meshNormal;
                        FbxGeometryElementNormal* pNormalElement = pMesh->GetElementNormal( 0 );
                        switch ( pNormalElement->GetMappingMode() )
                        {
                            case FbxGeometryElement::eByControlPoint:
                            switch ( pNormalElement->GetReferenceMode() )
                            {
                                case FbxGeometryElement::eDirect:
                                {
                                    meshNormal = pNormalElement->GetDirectArray().GetAt( ctrlPointIdx );
                                }
                                break;

                                case FbxGeometryElement::eIndexToDirect:
                                {
                                    int32 const normalIdx = pNormalElement->GetIndexArray().GetAt( ctrlPointIdx );
                                    meshNormal = pNormalElement->GetDirectArray().GetAt( normalIdx );
                                }
                                break;
                            }
                            break;

                            case FbxGeometryElement::eByPolygonVertex:
                            switch ( pNormalElement->GetReferenceMode() )
                            {
                                case FbxGeometryElement::eDirect:
                                {
                                    meshNormal = pNormalElement->GetDirectArray().GetAt( vertexIdx );
                                }
                                break;

                                case FbxGeometryElement::eIndexToDirect:
                                {
                                    int32 const normalIdx = pNormalElement->GetIndexArray().GetAt( vertexIdx );
                                    meshNormal = pNormalElement->GetDirectArray().GetAt( normalIdx );
                                }
                                break;
                            }
                            break;
                        }

                        meshNormal = meshNodeGlobalTransform.MultT( meshNormal );
                        vert.m_normal = Vector( (float) meshNormal[0], (float) meshNormal[1], (float) meshNormal[2] ).GetNormalized3();

                        // Get vertex tangent
                        //-------------------------------------------------------------------------

                        FbxVector4 meshTangent;
                        for ( auto l = 0; l < pMesh->GetElementTangentCount(); ++l )
                        {
                            FbxGeometryElementTangent* pTangent = pMesh->GetElementTangent( l );

                            if ( pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
                            {
                                switch ( pTangent->GetReferenceMode() )
                                {
                                    case FbxGeometryElement::eDirect:
                                    {
                                        meshTangent = pTangent->GetDirectArray().GetAt( vertexIdx );
                                    }
                                    break;

                                    case FbxGeometryElement::eIndexToDirect:
                                    {
                                        int32 const tangentIdx = pTangent->GetIndexArray().GetAt( vertexIdx );
                                        meshTangent = pTangent->GetDirectArray().GetAt( tangentIdx );
                                    }
                                    break;
                                }
                            }
                        }

                        meshTangent = meshNodeGlobalTransform.MultT( meshTangent );
                        vert.m_tangent = Vector( (float) meshTangent[0], (float) meshTangent[1], (float) meshTangent[2] ).GetNormalized3();

                        // Get vertex UV
                        //-------------------------------------------------------------------------

                        int32 const numUVChannelsForMeshSection = pMesh->GetElementUVCount();
                        for ( auto i = 0; i < numUVChannelsForMeshSection; ++i )
                        {
                            FbxGeometryElementUV* pTexcoordElement = pMesh->GetElementUV( i );
                            FbxVector2 texCoord( 0, 0 );

                            switch ( pTexcoordElement->GetMappingMode() )
                            {
                                case FbxGeometryElement::eByControlPoint:
                                {
                                    switch ( pTexcoordElement->GetReferenceMode() )
                                    {
                                        case FbxLayerElementUV::eDirect:
                                        {
                                            texCoord = pTexcoordElement->GetDirectArray().GetAt( vertexIdx );
                                        }
                                        break;

                                        case FbxLayerElementUV::eIndexToDirect:
                                        {
                                            int32 const texCoordIdx = pTexcoordElement->GetIndexArray().GetAt( vertexIdx );
                                            texCoord = pTexcoordElement->GetDirectArray().GetAt( texCoordIdx );
                                        }
                                        break;
                                    }
                                }
                                break;

                                case FbxGeometryElement::eByPolygonVertex:
                                {
                                    switch ( pTexcoordElement->GetReferenceMode() )
                                    {
                                        case FbxLayerElementUV::eDirect:
                                        {
                                            int32 const textureUVIndex = pMesh->GetTextureUVIndex( polygonIdx, vertexIdx );
                                            texCoord = pTexcoordElement->GetDirectArray().GetAt( textureUVIndex );
                                        }
                                        break;

                                        case FbxLayerElementUV::eIndexToDirect:
                                        {
                                            int32 const textureUVIndex = pMesh->GetTextureUVIndex( polygonIdx, vertexIdx );
                                            texCoord = pTexcoordElement->GetDirectArray().GetAt( textureUVIndex );
                                        }
                                        break;
                                    }
                                }
                                break;
                            }

                            vert.m_texCoords.push_back( Float2( (float) texCoord[0], 1.0f - (float) texCoord[1] ) );
                        }

                        // Add vertex to mesh data
                        //-------------------------------------------------------------------------

                        // Check whether we are referencing a new vertex or should use the index to an existing one?
                        uint32 existingVertexIdx = (uint32) InvalidIndex;
                        auto& vertexIndices = controlPointVertexMapping[ctrlPointIdx];
                        for ( auto const& idx : vertexIndices )
                        {
                            if ( vert == geometryData.m_vertices[idx] )
                            {
                                existingVertexIdx = idx;
                                break;
                            }
                        }

                        // The vertex already exists, so just add its index
                        if ( existingVertexIdx != (uint32) InvalidIndex )
                        {
                            KRG_ASSERT( existingVertexIdx < geometryData.m_vertices.size() );
                            geometryData.m_indices.push_back( existingVertexIdx );
                        }
                        else // Create new vertex
                        {
                            geometryData.m_indices.push_back( (uint32) geometryData.m_vertices.size() );
                            vertexIndices.push_back( (uint32) geometryData.m_vertices.size() );
                            geometryData.m_vertices.push_back( vert );
                        }
                    }
                }

                return true;
            }

            static FbxNode* FindSkeletonForSkin( FbxSkin* pSkin)
            {
                FbxNode* pSkeletonRootNode = nullptr;

                //-------------------------------------------------------------------------

                FbxCluster* pCluster = pSkin->GetCluster( 0 );
                FbxNode* pBoneNode = pCluster->GetLink();

                while ( pBoneNode != nullptr )
                {
                    // Traverse hierarchy to find the root of the skeleton
                    FbxNodeAttribute* pNodeAttribute = pBoneNode->GetNodeAttribute();
                    if ( pNodeAttribute != nullptr && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton )
                    {
                        pSkeletonRootNode = pBoneNode;
                        pBoneNode = pBoneNode->GetParent();
                    }
                    else
                    {
                        pBoneNode = nullptr;
                    }
                }

                return pSkeletonRootNode;
            }

            static bool ReadSkinningData( FbxRawMesh& rawMesh, Fbx::FbxSceneContext const& sceneCtx, fbxsdk::FbxMesh* pMesh, RawMesh::GeometrySection& geometryData, TVector<TVector<uint32>>& controlPointVertexMapping )
            {
                KRG_ASSERT( pMesh != nullptr && pMesh->IsTriangleMesh() && rawMesh.m_isSkeletalMesh );

                // Check whether skinning data exists for this mesh
                int32 const numSkins = pMesh->GetDeformerCount( FbxDeformer::eSkin );
                if ( numSkins == 0 )
                {
                    rawMesh.LogError( "No skin data found!" );
                    return false;
                }

                if ( numSkins > 1 )
                {
                    rawMesh.LogWarning( "More than one skin found for mesh (%s), compiling only the first skin found!", geometryData.m_name.c_str() );
                }

                // Get the final global transform of the mesh
                //-------------------------------------------------------------------------

                FbxVector4 const gT = pMesh->GetNode()->GetGeometricTranslation( FbxNode::eSourcePivot );
                FbxVector4 const gR = pMesh->GetNode()->GetGeometricRotation( FbxNode::eSourcePivot );
                FbxVector4 const gS = pMesh->GetNode()->GetGeometricScaling( FbxNode::eSourcePivot );
                FbxAMatrix geometricTransform( gT, gR, gS );

                // Read skin
                //-------------------------------------------------------------------------

                FbxSkin* pSkin = static_cast<FbxSkin*>( pMesh->GetDeformer( 0, FbxDeformer::eSkin ) );
                FbxSkin::EType const skinningType = pSkin->GetSkinningType();
                if ( skinningType != FbxSkin::eRigid && skinningType != FbxSkin::eLinear )
                {
                    rawMesh.LogError( "Unsupported skinning type for mesh (%s), only rigid and linear skinning supported!", geometryData.m_name.c_str() );
                    return false;
                }

                // Read skeleton
                //-------------------------------------------------------------------------

                auto pSkeletonNode = FindSkeletonForSkin( pSkin );
                if ( pSkeletonNode == nullptr )
                {
                    rawMesh.LogError( "Couldn't find skeleton for mesh!", geometryData.m_name.c_str() );
                    return false;
                }

                // Check if we have already read a skeleton for this mesh and if the skeleton matches
                if ( rawMesh.m_skeleton.IsValid() )
                {
                    StringID const skeletonRootName( (char const*) pSkeletonNode->GetNameWithoutNameSpacePrefix() );
                    if ( rawMesh.GetSkeleton().GetRootBoneName() != skeletonRootName )
                    {
                        rawMesh.LogError( "Different skeletons detected for the various sub-meshes. Expected: %s, Got: %s", rawMesh.GetSkeleton().GetRootBoneName().c_str(), skeletonRootName.c_str() );
                        return false;
                    }
                }
                else // Try to read the skeleton
                {
                    Fbx::ReadSkeleton( sceneCtx, (char const*) pSkeletonNode->GetNameWithoutNameSpacePrefix(), rawMesh.m_skeleton );
                    if ( !rawMesh.m_skeleton.IsValid() )
                    {
                        rawMesh.LogError( "Failed to read mesh skeleton", geometryData.m_name.c_str() );
                        return false;
                    }
                }

                // Read skinning cluster data (i.e. bone skin mapping)
                //-------------------------------------------------------------------------

                auto const numVertices = geometryData.m_vertices.size();
                TVector<TVector<TPair<uint16, float>>> vertexSkinMapping( numVertices );

                auto const numClusters = pSkin->GetClusterCount();
                for ( auto c = 0; c < numClusters; c++ )
                {
                    FbxCluster* pCluster = pSkin->GetCluster( c );
                    FbxNode const* pBoneNode = pCluster->GetLink();

                    StringID const boneName( pBoneNode->GetNameWithoutNameSpacePrefix() );
                    if ( pBoneNode == nullptr )
                    {
                        rawMesh.LogError( "No node linked to cluster %s", (char const*) pCluster->GetNameWithoutNameSpacePrefix() );
                        return false;
                    }

                    FbxCluster::ELinkMode const mode = pCluster->GetLinkMode();
                    if ( mode == FbxCluster::eAdditive )
                    {
                        rawMesh.LogError( "Unsupported link mode for joint: %s", boneName.c_str() );
                        return false;
                    }

                    // Find bone in skeleton that matches this cluster name
                    int32 const boneIdx = rawMesh.m_skeleton.GetBoneIndex( boneName );
                    if ( boneIdx == InvalidIndex )
                    {
                        rawMesh.LogError( "Unknown bone link node encountered in FBX scene (%s)", boneName.c_str() );
                        return false;
                    }

                    //-------------------------------------------------------------------------

                    FbxAMatrix clusterTransform, clusterLinkTransform;
                    pCluster->GetTransformMatrix( clusterTransform );
                    pCluster->GetTransformLinkMatrix( clusterLinkTransform );
                    FbxAMatrix const inverseBindPoseTransform = clusterLinkTransform.Inverse() * clusterTransform * geometricTransform;

                    FbxRawSkeleton& rawSkeleton = static_cast<FbxRawSkeleton&>( rawMesh.m_skeleton );
                    rawSkeleton.m_bones[boneIdx].m_globalTransform = sceneCtx.ConvertMatrixToTransform( inverseBindPoseTransform.Inverse() );
                    rawSkeleton.m_bones[boneIdx].m_globalTransform.SetTranslation( rawSkeleton.m_bones[boneIdx].m_globalTransform.GetTranslation() * sceneCtx.GetScaleConversionMultiplier() );

                    // Add bone indices and weight to all influenced vertices
                    auto const* pControlPointIndices = pCluster->GetControlPointIndices();
                    auto const* pControlPointWeights = pCluster->GetControlPointWeights();
                    auto const numControlPointIndices = pCluster->GetControlPointIndicesCount();

                    for ( auto i = 0; i < numControlPointIndices; i++ )
                    {
                        KRG_ASSERT( pControlPointIndices[i] < pMesh->GetControlPointsCount() );
                        if ( pControlPointWeights[i] > 0.f )
                        {
                            auto const& vertexIndices = controlPointVertexMapping[pControlPointIndices[i]];
                            for ( auto v = 0; v < vertexIndices.size(); v++ )
                            {
                                auto const vertexIdx = vertexIndices[v];
                                geometryData.m_vertices[vertexIdx].m_boneIndices.push_back( boneIdx );
                                geometryData.m_vertices[vertexIdx].m_boneWeights.push_back( (float) pControlPointWeights[i] );
                            }
                        }
                    }
                }

                // Ensure we have <= the max number of skinning influences per vertex
                //-------------------------------------------------------------------------

                bool vertexInfluencesReduced = false;
                auto const numVerts = geometryData.m_vertices.size();
                for ( auto v = 0; v < numVerts; v++ )
                {
                    auto& vert = geometryData.m_vertices[v];
                    auto numInfluences = vert.m_boneIndices.size();

                    if ( numInfluences > rawMesh.m_maxNumberOfBoneInfluences )
                    {
                        while ( vert.m_boneIndices.size() > rawMesh.m_maxNumberOfBoneInfluences )
                        {
                            // Remove lowest influence
                            int32 smallestWeightIdx = 0;
                            for ( auto f = 1; f < vert.m_boneIndices.size(); f++ )
                            {
                                if ( vert.m_boneWeights[f] < vert.m_boneWeights[smallestWeightIdx] )
                                {
                                    smallestWeightIdx = f;
                                }
                            }

                            vert.m_boneWeights.erase_unsorted( vert.m_boneWeights.begin() + smallestWeightIdx );
                            vert.m_boneIndices.erase_unsorted( vert.m_boneIndices.begin() + smallestWeightIdx );
                        }

                        vertexInfluencesReduced = true;

                        // Re-normalize weights
                        auto const totalWeight = vert.m_boneWeights[0] + vert.m_boneWeights[1] + vert.m_boneWeights[2] + vert.m_boneWeights[3];
                        for ( auto i = 1; i < rawMesh.m_maxNumberOfBoneInfluences; i++ )
                        {
                            vert.m_boneWeights[i] /= totalWeight;
                        }
                    }
                }

                if ( vertexInfluencesReduced )
                {
                    rawMesh.LogWarning( "More than %d skinning influences detected per bone for mesh (%s), this is not supported - influences have been reduced to %d", rawMesh.m_maxNumberOfBoneInfluences, geometryData.m_name.c_str(), rawMesh.m_maxNumberOfBoneInfluences );
                }

                return true;
            }
        };
    }

    //-------------------------------------------------------------------------

    TUniquePtr<RawAssets::RawMesh> Fbx::ReadStaticMesh( FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile )
    {
        return RawAssets::FbxMeshFileReader::ReadStaticMesh( sourceFilePath, nameOfMeshToCompile );
    }

    TUniquePtr<RawAssets::RawMesh> Fbx::ReadSkeletalMesh( FileSystem::Path const& sourceFilePath, int32 maxBoneInfluences )
    {
        return RawAssets::FbxMeshFileReader::ReadSkeletalMesh( sourceFilePath, maxBoneInfluences );
    }
}