#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_TOOLS_PHYSICS_API PhysicsMaterialDatabaseResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( PhysicsMaterialDatabaseResourceDescriptor );

        KRG_EXPOSE TVector<ResourcePath>         m_materialLibraries;
    };
}