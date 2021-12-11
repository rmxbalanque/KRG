#pragma once

#include "System/Core/Math/BoundingVolumes.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Drawing { class DrawContext; }

    //-------------------------------------------------------------------------

    namespace Math
    {
        class KRG_SYSTEM_CORE_API AABBTree
        {
            struct Node
            {
            public:

                Node() = default;
                Node( AABB&& bounds, uint64 userData = 0 ) : m_bounds( bounds ), m_userData( userData ), m_volume( bounds.GetVolume() ) {}
                Node( AABB const& bounds, uint64 userData = 0 ) : m_bounds( bounds ), m_userData( userData ), m_volume( bounds.GetVolume() ) {}

                inline bool IsLeafNode() const { return m_rightNodeIdx == InvalidIndex; }

            public:

                AABB            m_bounds = AABB( Vector::Zero );

                int32           m_leftNodeIdx = InvalidIndex;
                int32           m_rightNodeIdx = InvalidIndex;
                int32           m_parentNodeIdx = InvalidIndex;
                float           m_volume = 0;

                uint64          m_userData = 0xFFFFFFFFFFFFFFFF;
                bool            m_isFree = true;
            };

        public:

            AABBTree();

            inline bool IsEmpty() const { return m_rootNodeIdx == InvalidIndex; }

            void InsertBox( AABB const& aabb, uint64 userData );
            void RemoveBox( uint64 userData );

            KRG_FORCE_INLINE void InsertBox( AABB const& aabb, void* pUserData ) { InsertBox( aabb, reinterpret_cast<uint64>( pUserData ) ); }
            KRG_FORCE_INLINE void RemoveBox( void* pUserData ) { RemoveBox( reinterpret_cast<uint64>( pUserData ) ); }

            bool FindOverlaps( AABB const& queryBox, TVector<uint64>& outResults ) const;

            template<typename T>
            bool FindOverlaps( AABB const& queryBox, TVector<T*>& outResults ) const
            {
                return FindOverlaps( queryBox, reinterpret_cast<TVector<uint64>&>( outResults ) );
            }

            #if KRG_DEVELOPMENT_TOOLS
            void DrawDebug( Drawing::DrawContext& drawingContext ) const;
            #endif

        private:

            void InsertNode( int32 leafNodeIdx, AABB const& newSiblingBox, uint64 userData );
            void RemoveNode( int32 nodeToRemoveIdx );
            void UpdateBranchNodeBounds( int32 nodeIdx );

            int32 RequestNode( AABB const& box, uint64 userData = 0 );
            void ReleaseNode( int32 nodeIdx );

            int32 FindBestLeafNodeToCreateSiblingFor( int32 startNodeIdx, AABB const& newBox ) const;
            void FindAllOverlappingLeafNodes( int32 currentNodeIdx, AABB const& queryBox, TVector<uint64>& outResults ) const;

            #if KRG_DEVELOPMENT_TOOLS
            void DrawBranch( Drawing::DrawContext& drawingContext, int32 nodeIdx ) const;
            void DrawLeaf( Drawing::DrawContext& drawingContext, int32 nodeIdx ) const;
            #endif

        private:

            TVector<Node>       m_nodes;
            int32               m_rootNodeIdx = InvalidIndex;
            int32               m_freeNodeIdx = 0;
        };
    }
}

