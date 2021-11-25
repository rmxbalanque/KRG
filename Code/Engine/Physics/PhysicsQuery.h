#pragma once

#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    //-------------------------------------------------------------------------
    // Results from a given query
    //-------------------------------------------------------------------------

    struct RayCastResults : public physx::PxRaycastBufferN<32> {};
    struct ShapeCastResults : public physx::PxSweepBufferN<32> {};
    struct OverlapResults : public physx::PxOverlapBufferN<32> {};

    //-------------------------------------------------------------------------
    // PhysX Query Filter
    //-------------------------------------------------------------------------
    // Helper to abstract some PhysX complexity, and to provide syntactic sugar
    // Currently word0 in the filter data is used to specify the layer to query against (all other words are currently left unset)
    // For the currently define layers in the engine, please refer to "PhysicsLayers.h"

    class QueryFilter final : public physx::PxQueryFilterCallback
    {
    public:

        enum class MobilityFilter
        {
            None,
            IgnoreStatic,
            IgnoreDynamic
        };

    public:

        // By default queries will collide against ALL static and dynamic actors 
        // The filter data is set by default to zero which skips the PxFilterData step - so will collide with all layers
        QueryFilter()
            : m_filterData( physx::PxQueryFilterData( physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER ) )
        {}

        // Create a query specifying the layer mask to use for filtering
        QueryFilter( uint32 layerMask )
            : m_filterData( physx::PxQueryFilterData( physx::PxFilterData( layerMask, 0, 0, 0 ), physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER ) )
        {}

        // Layer Filtering
        //-------------------------------------------------------------------------
        // This allows you to specify what layers this query will be run against (e.g. collide with only the environment or with characters, etc... )

        void SetLayerMask( uint32 layerMask )
        {
            m_filterData.data.word0 = layerMask;
        }

        // Mobility Filtering
        //-------------------------------------------------------------------------
        // By default queries will collide against both static and dynamic actors

        inline bool IsQueryingStaticActors()
        {
            return m_filterData.flags.isSet( physx::PxQueryFlag::eSTATIC );
        }

        inline bool IsQueryingDynamicActors()
        {
            return m_filterData.flags.isSet( physx::PxQueryFlag::eDYNAMIC );
        }

        inline void SetMobilityFilter( MobilityFilter filter )
        {
            switch ( filter )
            {
                case MobilityFilter::None:
                m_filterData.flags |= physx::PxQueryFlag::eSTATIC;
                m_filterData.flags |= ~physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreStatic:
                m_filterData.flags &= ~physx::PxQueryFlag::eSTATIC;
                m_filterData.flags |= physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreDynamic:
                m_filterData.flags |= physx::PxQueryFlag::eSTATIC;
                m_filterData.flags &= ~physx::PxQueryFlag::eDYNAMIC;
                break;
            }
        }

    private:

        virtual physx::PxQueryHitType::Enum preFilter( physx::PxFilterData const& filterData, physx::PxShape const* pShape, physx::PxRigidActor const* pActor, physx::PxHitFlags& queryFlags ) override;
        virtual physx::PxQueryHitType::Enum postFilter( physx::PxFilterData const& filterData, physx::PxQueryHit const& hit ) override;

    public:

        physx::PxQueryFilterData                    m_filterData;
        TInlineVector<physx::PxRigidActor*,4>       m_ignoredActors;
    };
}