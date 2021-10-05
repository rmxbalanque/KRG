#pragma once

#include "_Module/API.h"
#include "PhysicsMaterial.h"
#include "System/Resource/IResource.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    // Empty resource - acts as a placeholder for the actual data being loaded - see PhysicsMaterialDatabaseLoader for details
    class KRG_ENGINE_PHYSICS_API PhysicsMaterialDatabase final : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'PMDB', "Physics Material DB" );
        friend class PhysicsMaterialDatabaseCompiler;
        friend class PhysicsMaterialDatabaseLoader;

        KRG_SERIALIZE_NONE();

    public:

        bool IsValid() const override final { return true; }
    };
}