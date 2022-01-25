#pragma once

#include "../_Module/API.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "System/Resource/ResourcePtr.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    struct NavmeshBuildSettings : public IRegisteredType
    {
        KRG_REGISTER_TYPE( NavmeshBuildSettings );

        KRG_EXPOSE StringID                         m_layerID;
        KRG_EXPOSE float                            m_voxSize = 0.1f; // Size of auto-generation voxels
        KRG_EXPOSE float                            m_radius = 0.45f; // Body radius of the creatures which will use this NavGraph.  The NavGraph will be retracted by approximately this radius away from walls.
        KRG_EXPOSE float                            m_dropOffRadius = 0.2f;// The distance that the NavGraph should be retracted from dropOffs (a.k.a. ledges or falling edges).  This value can be slightly smaller than the m_radius.
        KRG_EXPOSE float                            m_step = 0.2f; // Maximum step height for surface movement
        KRG_EXPOSE float                            m_height = 2.0f; // Clearance height for the creatures which will use this NavGraph
        KRG_EXPOSE int                              m_maxNumIslands = 0; // The build process will enforce this optional limit by removing small islands. A value of 0 disables the feature, i.e. no islands are removed.
        KRG_EXPOSE float                            m_minIslandSurfaceArea = 2.0f; // The build process will eliminate any islands with a surface area smaller than this limit (distance units squared)
        KRG_EXPOSE bool                             m_leaveSmallIslandsTouchingPortals = true; // If true then the build process will ignore m_maxNumIslands and m_minIslandSurfaceArea for islands touching a streaming portal.
        KRG_EXPOSE float                            m_additionalInwardsSmoothingDist = 0.0f; // The build process will perform additional smoothing on the NavGraph edges, moving vertices inwards by up to this distance.
        KRG_EXPOSE float                            m_maxWalkableSlope = 40.0f;	// The maximum slope (degrees) for a BuildFace of type WALKABLE_FACE.  Faces steeper than this will be treated as being of type OBSTACLE_FACE.
        KRG_EXPOSE float                            m_verticalOffsetDist = 0.1f; // The auto-gen NavGraph will be offset vertically by this additional distance.  Positive values move the NavGraph upwards.
        KRG_EXPOSE bool                             m_useEnhancedTerrainTracking = true; // The auto-gen NavGraph will track the terrain more closely, especially in hilly areas.
        KRG_EXPOSE bool                             m_optimizeForAxisAligned = false; // Optimizes the NavGraph assuming that the level contains mostly axis aligned walls, obstacles, etc.
        KRG_EXPOSE bool                             m_tessellateForPathingAccuracy = false; // Tessellate the NavGraph for improved accuracy at runtime at the cost of a slight run-time performance penalty.
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( NavmeshComponent );
        friend class NavmeshWorldSystem;
        friend class NavmeshBuilder;

    public:

        inline bool HasNavmeshData() const { return m_pNavmeshData.IsLoaded() && m_pNavmeshData->IsValid(); }

    private:

        KRG_EXPOSE NavmeshBuildSettings             m_defaultLayerBuildSettings;
        KRG_EXPOSE TVector<NavmeshBuildSettings>    m_additionalLayerBuildSettings;
        KRG_EXPOSE bool                             m_enableBuildLogging = false;

        KRG_EXPOSE TResourcePtr<NavmeshData>        m_pNavmeshData;
    };
}