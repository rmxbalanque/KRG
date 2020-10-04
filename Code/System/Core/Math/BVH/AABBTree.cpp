#include "AABBTree.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Debug/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        AABBTree::AABBTree()
        {
            m_nodes.resize( 100 );
        }

        //-------------------------------------------------------------------------

        S32 AABBTree::FindBestLeafNodeToCreateSiblingFor( S32 startNodeIdx, AABB const& newBox ) const
        {
            auto const& startNode = m_nodes[startNodeIdx];
            KRG_ASSERT( !startNode.IsLeafNode() && startNode.m_leftNodeIdx != InvalidIndex );

            S32 currentNodeIdx = startNodeIdx;
            while ( currentNodeIdx != InvalidIndex )
            {
                auto const& currentNode = m_nodes[currentNodeIdx];
                auto const& leftNode = m_nodes[currentNode.m_leftNodeIdx];
                auto const& rightNode = m_nodes[currentNode.m_rightNodeIdx];

                F32 const leftVolume = leftNode.m_bounds.GetMergedBox( newBox ).GetVolume();
                F32 const rightVolume = rightNode.m_bounds.GetMergedBox( newBox ).GetVolume();

                // This currently aims to minimizes the volume that would be created from the sibling pair
                if ( leftVolume <= rightVolume )
                {
                    if ( leftNode.IsLeafNode() )
                    {
                        return currentNode.m_leftNodeIdx;
                    }
                    else
                    {
                        currentNodeIdx = currentNode.m_leftNodeIdx;
                    }
                }
                else
                {
                    if ( rightNode.IsLeafNode() )
                    {
                        return currentNode.m_rightNodeIdx;
                    }
                    else
                    {
                        currentNodeIdx = currentNode.m_rightNodeIdx;
                    }
                }
            }

            KRG_UNREACHABLE_CODE();
            return InvalidIndex;
        }

        void AABBTree::InsertBox( AABB const& newBox, U64 userData )
        {
            KRG_ASSERT( newBox.IsValid() );

            // All boxes must have a non-zero unique userdata value as that is also used as the ID
            KRG_ASSERT( userData != 0 && !VectorContains( m_nodes, userData, [] ( Node const& node, U64 userData ) { return !node.m_isFree && node.IsLeafNode() && node.m_userData == userData; } ) );

            // First box
            if ( m_rootNodeIdx == InvalidIndex )
            {
                RequestNode( newBox, userData );
                m_rootNodeIdx = 0;
            }
            // If the root node is a leaf, the new box is a sibling
            else if ( m_nodes[m_rootNodeIdx].IsLeafNode() )
            {
                InsertNode( m_rootNodeIdx, newBox, userData );
            }
            else // Find the best leaf node to create a sibling to
            {
                U32 bestNodeIdx = FindBestLeafNodeToCreateSiblingFor( m_rootNodeIdx, newBox );
                KRG_ASSERT( bestNodeIdx != InvalidIndex );
                InsertNode( bestNodeIdx, newBox, userData );
            }
        }

        void AABBTree::UpdateBranchNodeBounds( S32 nodeIdx )
        {
            auto& currentNode = m_nodes[nodeIdx];
            KRG_ASSERT( !currentNode.IsLeafNode() );

            currentNode.m_bounds = m_nodes[currentNode.m_leftNodeIdx].m_bounds.GetMergedBox( m_nodes[currentNode.m_rightNodeIdx].m_bounds );
            currentNode.m_volume = currentNode.m_bounds.GetVolume();
        }

        void AABBTree::InsertNode( S32 originalLeafNodeIdx, AABB const& newSiblingBox, U64 userData )
        {
            KRG_ASSERT( newSiblingBox.IsValid() );

            auto const& originalLeafNode = m_nodes[originalLeafNodeIdx];
            S32 const grandparentIdx = originalLeafNode.m_parentNodeIdx;

            //-------------------------------------------------------------------------

            // Create new branch node
            S32 const newBranchNodeIdx = RequestNode( newSiblingBox );
            m_nodes[newBranchNodeIdx].m_parentNodeIdx = grandparentIdx;

            // Set left child to original leaf node
            m_nodes[newBranchNodeIdx].m_leftNodeIdx = originalLeafNodeIdx;
            m_nodes[m_nodes[newBranchNodeIdx].m_leftNodeIdx].m_parentNodeIdx = newBranchNodeIdx;

            // Create the sibling node and set it as the right child
            S32 const newSiblingNodeIdx = RequestNode( newSiblingBox, userData );
            m_nodes[newBranchNodeIdx].m_rightNodeIdx = newSiblingNodeIdx;
            m_nodes[m_nodes[newBranchNodeIdx].m_rightNodeIdx].m_parentNodeIdx = newBranchNodeIdx;

            // Update the bounds of the new branch node
            UpdateBranchNodeBounds( newBranchNodeIdx );

            // Update the grandparent node to point to the newly create branch node
            if ( grandparentIdx != InvalidIndex )
            {
                if ( m_nodes[grandparentIdx].m_leftNodeIdx == originalLeafNodeIdx )
                {
                    m_nodes[grandparentIdx].m_leftNodeIdx = newBranchNodeIdx;
                }
                else
                {
                    m_nodes[grandparentIdx].m_rightNodeIdx = newBranchNodeIdx;
                }
            }
            else
            {
                m_rootNodeIdx = newBranchNodeIdx;
            }

            // Propagate changes up the hierarchy
            S32 parentIndex = grandparentIdx;
            while ( parentIndex != InvalidIndex )
            {
                UpdateBranchNodeBounds( parentIndex );
                parentIndex = m_nodes[parentIndex].m_parentNodeIdx;
            }
        }

        void AABBTree::RemoveBox( U64 userData )
        {
            S32 const nodeToRemoveIdx = VectorFindIndex( m_nodes, userData, [] ( Node const& node, U64 userData ) { return !node.m_isFree && node.IsLeafNode() && node.m_userData == userData; } );
            KRG_ASSERT( nodeToRemoveIdx != InvalidIndex && m_nodes[nodeToRemoveIdx].IsLeafNode() );
            RemoveNode( nodeToRemoveIdx );
        }

        void AABBTree::RemoveNode( S32 nodeToRemoveIdx )
        {
            // Check if we are the root node
            S32 const parentNodeIdx = m_nodes[nodeToRemoveIdx].m_parentNodeIdx;
            if ( parentNodeIdx == InvalidIndex )
            {
                KRG_ASSERT( m_rootNodeIdx == nodeToRemoveIdx );
                m_rootNodeIdx = InvalidIndex;
                ReleaseNode( nodeToRemoveIdx );
            }
            else // Replace the parent branch node with our sibling
            {
                S32 const siblingIdx = ( m_nodes[parentNodeIdx].m_leftNodeIdx == nodeToRemoveIdx ) ? m_nodes[parentNodeIdx].m_rightNodeIdx : m_nodes[parentNodeIdx].m_leftNodeIdx;

                // If we dont have a grandparent then the parent branch was the root
                S32 const grandparentNodeIdx = m_nodes[parentNodeIdx].m_parentNodeIdx;
                if ( grandparentNodeIdx == InvalidIndex )
                {
                    KRG_ASSERT( m_rootNodeIdx == parentNodeIdx );
                    m_nodes[siblingIdx].m_parentNodeIdx = InvalidIndex;
                    m_rootNodeIdx = siblingIdx;
                }
                else // Set indices so that sibling is a child of the grandparent
                {
                    if ( m_nodes[grandparentNodeIdx].m_leftNodeIdx == parentNodeIdx )
                    {
                        m_nodes[grandparentNodeIdx].m_leftNodeIdx = siblingIdx;
                    }
                    else
                    {
                        m_nodes[grandparentNodeIdx].m_rightNodeIdx = siblingIdx;
                    }

                    m_nodes[siblingIdx].m_parentNodeIdx = grandparentNodeIdx;

                    // Propagate changes up the hierarchy
                    S32 parentIndex = grandparentNodeIdx;
                    while ( parentIndex != InvalidIndex )
                    {
                        UpdateBranchNodeBounds( parentIndex );
                        parentIndex = m_nodes[parentIndex].m_parentNodeIdx;
                    }
                }

                // Release nodes and set free node index
                ReleaseNode( parentNodeIdx );
                ReleaseNode( nodeToRemoveIdx );
            }
        }

        //-------------------------------------------------------------------------

        S32 AABBTree::RequestNode( AABB const& box, U64 userData )
        {
            KRG_ASSERT( m_nodes[m_freeNodeIdx].m_isFree );

            // Create a new node in the first free idx
            S32 freeNodeIdx = m_freeNodeIdx;
            new ( &m_nodes[freeNodeIdx] ) Node( box, userData );
            m_nodes[freeNodeIdx].m_isFree = false;

            // Try to find the next free node idx
            for ( ++m_freeNodeIdx; m_freeNodeIdx < m_nodes.size(); m_freeNodeIdx++ )
            {
                if ( m_nodes[m_freeNodeIdx].m_isFree )
                {
                    break;
                }
            }

            // Allocate additional node memory
            if ( m_freeNodeIdx == m_nodes.size() )
            {
                m_nodes.resize( Math::FloorToInt( m_nodes.size() * 1.25f ) );
            }

            return freeNodeIdx;
        }

        void AABBTree::ReleaseNode( S32 nodeIdx )
        {
            KRG_ASSERT( nodeIdx >= 0 && nodeIdx < m_nodes.size() && !m_nodes[nodeIdx].m_isFree );
            m_nodes[nodeIdx].m_isFree = true;
            m_freeNodeIdx = Math::Min( m_freeNodeIdx, nodeIdx );
        }

        //-------------------------------------------------------------------------

        void AABBTree::FindAllOverlappingLeafNodes( S32 currentNodeIdx, AABB const& queryBox, TVector<U64>& outResults ) const
        {
            Node const& currentNode = m_nodes[currentNodeIdx];
            if ( currentNode.IsLeafNode() )
            {
                if ( currentNode.m_bounds.Overlaps( queryBox ) )
                {
                    KRG_ASSERT( currentNode.m_userData != 0 );
                    outResults.push_back( currentNode.m_userData );
                }
            }
            else
            {
                FindAllOverlappingLeafNodes( currentNode.m_leftNodeIdx, queryBox, outResults );
                FindAllOverlappingLeafNodes( currentNode.m_rightNodeIdx, queryBox, outResults );
            }
        }

        bool AABBTree::FindOverlaps( AABB const& queryBox, TVector<U64>& outResults ) const
        {
            outResults.clear();

            if ( m_rootNodeIdx == InvalidIndex )
            {
                return false;
            }

            FindAllOverlappingLeafNodes( m_rootNodeIdx, queryBox, outResults );
            return outResults.size() > 0;
        }

        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        void AABBTree::DrawDebug( Debug::DrawingContext& drawingContext ) const
        {
            if ( m_rootNodeIdx == InvalidIndex )
            {
                return;
            }

            //-------------------------------------------------------------------------

            if ( m_nodes[m_rootNodeIdx].IsLeafNode() )
            {
                DrawLeaf( drawingContext, m_rootNodeIdx );
            }
            else
            {
                DrawBranch( drawingContext, m_rootNodeIdx );
            }
        }

        void AABBTree::DrawBranch( Debug::DrawingContext& drawingContext, S32 nodeIdx ) const
        {
            auto const& currentNode = m_nodes[nodeIdx];
            KRG_ASSERT( !currentNode.IsLeafNode() );
            drawingContext.DrawWireBox( currentNode.m_bounds, Colors::Cyan, 1.0f, Debug::Drawing::DepthTestState::On );

            // Left
            if ( m_nodes[currentNode.m_leftNodeIdx].IsLeafNode() )
            {
                DrawLeaf( drawingContext, currentNode.m_leftNodeIdx );
            }
            else
            {
                DrawBranch( drawingContext, currentNode.m_leftNodeIdx );
            }

            // Right
            if ( m_nodes[currentNode.m_rightNodeIdx].IsLeafNode() )
            {
                DrawLeaf( drawingContext, currentNode.m_rightNodeIdx );
            }
            else
            {
                DrawBranch( drawingContext, currentNode.m_rightNodeIdx );
            }
        }

        void AABBTree::DrawLeaf( Debug::DrawingContext& drawingContext, S32 nodeIdx ) const
        {
            KRG_ASSERT( m_nodes[nodeIdx].IsLeafNode() );
            drawingContext.DrawWireBox( m_nodes[nodeIdx].m_bounds, Colors::Lime, 2.0f, Debug::Drawing::DepthTestState::On );
        }
        #endif
    }
}