#pragma once

#include "System/Core/Math/BoundingVolumes.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Debug { class DrawingContext; }

    //-------------------------------------------------------------------------

    namespace Math
    {
        class KRG_SYSTEM_CORE_API AABBTree
        {
            struct Node
            {
            public:

                Node() = default;
                Node( AABB&& bounds, U64 userData = 0 ) : m_bounds( bounds ), m_userData( userData ), m_volume( bounds.GetVolume() ) {}
                Node( AABB const& bounds, U64 userData = 0 ) : m_bounds( bounds ), m_userData( userData ), m_volume( bounds.GetVolume() ) {}

                inline bool IsLeafNode() const { return m_rightNodeIdx == InvalidIndex; }

            public:

                AABB            m_bounds = AABB( Vector::Zero );

                S32             m_leftNodeIdx = InvalidIndex;
                S32             m_rightNodeIdx = InvalidIndex;
                S32             m_parentNodeIdx = InvalidIndex;
                F32             m_volume = 0;

                U64             m_userData = 0xFFFFFFFFFFFFFFFF;
                bool            m_isFree = true;
            };

        public:

            AABBTree();

            inline bool IsEmpty() const { return m_rootNodeIdx == InvalidIndex; }

            void InsertBox( AABB const& aabb, U64 userData );
            void RemoveBox( U64 userData );

            KRG_FORCE_INLINE void InsertBox( AABB const& aabb, void* pUserData ) { InsertBox( aabb, reinterpret_cast<U64>( pUserData ) ); }
            KRG_FORCE_INLINE void RemoveBox( void* pUserData ) { RemoveBox( reinterpret_cast<U64>( pUserData ) ); }

            bool FindOverlaps( AABB const& queryBox, TVector<U64>& outResults ) const;

            template<typename T>
            bool FindOverlaps( AABB const& queryBox, TVector<T*>& outResults ) const
            {
                return FindOverlaps( queryBox, reinterpret_cast<TVector<U64>&>( outResults ) );
            }

            #if KRG_DEVELOPMENT_TOOLS
            void DrawDebug( Debug::DrawingContext& drawingContext ) const;
            #endif

        private:

            void InsertNode( S32 leafNodeIdx, AABB const& newSiblingBox, U64 userData );
            void RemoveNode( S32 nodeToRemoveIdx );
            void UpdateBranchNodeBounds( S32 nodeIdx );

            S32 RequestNode( AABB const& box, U64 userData = 0 );
            void ReleaseNode( S32 nodeIdx );

            S32 FindBestLeafNodeToCreateSiblingFor( S32 startNodeIdx, AABB const& newBox ) const;
            void FindAllOverlappingLeafNodes( S32 currentNodeIdx, AABB const& queryBox, TVector<U64>& outResults ) const;

            #if KRG_DEVELOPMENT_TOOLS
            void DrawBranch( Debug::DrawingContext& drawingContext, S32 nodeIdx ) const;
            void DrawLeaf( Debug::DrawingContext& drawingContext, S32 nodeIdx ) const;
            #endif

        private:

            TVector<Node>   m_nodes;
            S32             m_rootNodeIdx = InvalidIndex;
            S32             m_freeNodeIdx = 0;
        };
    }
}

