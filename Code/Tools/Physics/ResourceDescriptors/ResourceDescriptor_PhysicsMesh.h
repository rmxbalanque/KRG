#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_TOOLS_PHYSICS_API PhysicsMeshResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( PhysicsMeshResourceDescriptor );

        KRG_EXPOSE ResourcePath     m_meshPath;

        // Optional value: Specifies a single sub-mesh to use to generated the physics collision, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
        KRG_EXPOSE String           m_meshName;

        // Optional value: Specifies if the mesh is a convex mesh (meshes are considered triangle meshes by default)
        KRG_EXPOSE bool             m_isConvexMesh = false;
    };
}