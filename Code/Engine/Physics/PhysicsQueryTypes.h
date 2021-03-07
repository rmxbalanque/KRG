#pragma once

#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    //-------------------------------------------------------------------------
    // Scene Query Rules
    //-------------------------------------------------------------------------
    // Helper to abstract some PhysX complexity, and to provide syntactic sugar

    struct QueryRules : public physx::PxQueryFilterData
    {
        enum class MobilityFilter
        {
            None,
            IgnoreStatic,
            IgnoreDynamic
        };

    public:

        // By default queries will collide against both static and dynamic actors
        QueryRules()
            : physx::PxQueryFilterData( physx::PxFilterData( 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF ), physx::PxQueryFlags( physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER ) )
        {}

        // Mobility Filtering
        //-------------------------------------------------------------------------
        // By default queries will collide against both static and dynamic actors

        inline bool IsQueryingStaticActors()
        {
            return flags.isSet( physx::PxQueryFlag::eSTATIC );
        }

        inline bool IsQueryingDynamicActors()
        {
            return flags.isSet( physx::PxQueryFlag::eDYNAMIC );
        }

        inline void SetMobilityFilter( MobilityFilter filter )
        {
            switch ( filter )
            {
                case MobilityFilter::None:
                flags |= physx::PxQueryFlag::eSTATIC;
                flags |= ~physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreStatic:
                flags &= ~physx::PxQueryFlag::eSTATIC;
                flags |= physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreDynamic:
                flags |= physx::PxQueryFlag::eSTATIC;
                flags &= ~physx::PxQueryFlag::eDYNAMIC;
                break;
            }
        }
    };

    //-------------------------------------------------------------------------
    // Results from a given query
    //-------------------------------------------------------------------------
    // Helper to abstract some PhysX complexity, and to provide syntactic sugar

    template<typename HitType>
    struct CastResults : public physx::PxHitBuffer<HitType>
    {
        constexpr static int32 const s_maxNumTouches = 16;

    public:

        CastResults() : physx::PxHitBuffer<HitType>( m_touches, s_maxNumTouches ) {}

    public:

        HitType         m_touches[s_maxNumTouches];
    };

    struct RayCastResults : public CastResults<physx::PxRaycastHit> {};
    struct ShapeCastResults : public CastResults<physx::PxSweepHit> {};
    struct OverlapResults : public CastResults<physx::PxOverlapHit> {};
}