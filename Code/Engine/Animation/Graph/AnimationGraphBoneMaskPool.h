#pragma once
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class Skeleton;
    class BoneMask;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class BoneMaskPool
    {
        constexpr static int32 const s_initialPoolSize = 4;

    public:

        BoneMaskPool() : m_firstFreePoolIdx( InvalidIndex ) {}
        ~BoneMaskPool();

        inline bool IsEmpty() const { return m_pool.size() == 0; }
        void Initialize( Skeleton const* pSkeleton );
        void Shutdown();

        BoneMask* GetBoneMask();
        void Reset() { m_firstFreePoolIdx = 0; }

    private:

        TVector<BoneMask*>      m_pool;
        int32                   m_firstFreePoolIdx = InvalidIndex;
    };
}