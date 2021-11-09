#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Physics/PhysicsMaterialDatabase.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_TOOLS_PHYSICS_API PhysicsMaterialDatabaseResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( PhysicsMaterialDatabaseResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const { return PhysicsMaterialDatabase::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE TVector<ResourcePath>         m_materialLibraries;
    };
}