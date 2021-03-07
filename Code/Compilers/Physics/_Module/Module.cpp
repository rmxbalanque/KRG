#if _WIN32
#include "Module.h"
#include "Compilers/Physics/PhysicsMeshCompiler.h"
#include "Compilers/Physics/PhysicsMaterialDatabaseCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Physics" )
        {
            AddCompiler( KRG::New<KRG::Physics::PhysicsMeshCompiler>() );
            AddCompiler( KRG::New<KRG::Physics::PhysicsMaterialDatabaseCompiler>() );
        }
    }
}

#endif