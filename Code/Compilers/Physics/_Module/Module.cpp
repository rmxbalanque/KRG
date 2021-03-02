#if _WIN32
#include "Module.h"
#include "Compilers/Physics/PhysicsMeshCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Physics" )
        {
            AddCompiler( KRG::New<KRG::Physics::PhysicsMeshCompiler>() );
        }
    }
}

#endif